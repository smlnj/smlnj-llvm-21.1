/// \file context.cpp
///
/// \copyright 2024 The Fellowship of SML/NJ (https://smlnj.org)
/// All rights reserved.
///
/// \brief This file implements the methods for the `Context` class
///
/// \author John Reppy
///

#include "context.hpp"
#include "target-info.hpp"
#include "mc-gen.hpp"
#include "cfg.hpp" // for argument setup
#include "object-file.hpp"

#include "llvm/IR/Constants.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_os_ostream.h"

#include <string>

namespace smlnj {
namespace cfgcg {

/* FIXME: for now, these are all zero, but we should do something else */
/* address spaces for various kinds of ML data that are necessarily disjoint */
#define ML_HEAP_ADDR_SP		0		// immutable heap objects
#define ML_REF_ADDR_SP		0		// mutable heap objects

/***** class Context member functions *****/

Context *Context::create (const TargetInfo *tgtInfo)
{
    Context *buf = new Context (tgtInfo);

    return buf;
}

Context *Context::create (std::string_view target)
{
    auto tgtInfo = TargetInfo::infoForTarget (target);
    if (tgtInfo == nullptr) {
	return nullptr;
    }

    Context *buf = new Context (tgtInfo);

    return buf;
}

Context::Context (TargetInfo const *target)
  : LLVMContext(),
    _target(target),
    _builder(*this),
    _gen(nullptr),
  // initialize the register info
    _regInfo(target),
    _regState(this->_regInfo)
{
    this->_gen = new MCGen (target),

  // initialize the standard types that we use
    this->i8Ty = llvm::IntegerType::get (*this, 8);
    this->i16Ty = llvm::IntegerType::get (*this, 16);
    this->i32Ty = llvm::IntegerType::get (*this, 32);
    this->i64Ty = llvm::IntegerType::get (*this, 64);
    this->f32Ty = llvm::Type::getPrimitiveType (*this, llvm::Type::FloatTyID);
    this->f64Ty = llvm::Type::getPrimitiveType (*this, llvm::Type::DoubleTyID);

    if (this->_target->wordSz == 32) {
	this->intTy = this->i32Ty;
	this->_wordSzB = 4;
    }
    else { // info.wordSz == 64
	this->intTy = this->i64Ty;
	this->_wordSzB = 8;
    }

    this->ptrTy = llvm::PointerType::getUnqual(*this);
    this->mlValueTy = this->ptrTy;
    this->voidTy = llvm::Type::getVoidTy (*this);

  // "call-gc" types
    {
	int n = target->numCalleeSaves + 4;
	Types_t gcTys = this->createParamTys (frag_kind::STD_FUN, n);
	for (int i = 0;  i < n;  ++i) {
	    gcTys.push_back (this->mlValueTy);
	}
	auto gcRetTy = llvm::StructType::create(gcTys, "gc_ret_ty");
	this->_gcFnTy = llvm::FunctionType::get(gcRetTy, gcTys, false);
    }

  // initialize the overflow block and raise_overflow function type
    {
      // the overflow block and raise_overflow have a minimal calling convention
      // that consists of just the hardware CMachine registers.  These are
      // necessary to ensure that the correct values are in place at the point
      // where the Overflow exception will be raised.
      //
	Types_t tys;
	int nArgs = this->_regInfo.numMachineRegs();
	tys.reserve (nArgs);
	for (int i = 0;  i < nArgs;  ++i) {
	    tys.push_back (this->ptrTy);
	}
	this->_raiseOverflowFnTy = llvm::FunctionType::get(this->voidTy, tys, false);
	this->_overflowBB = nullptr;
    }

    // initialize the cached intrinsic functions
    this->_sadd32WO.init(llvm::Intrinsic::sadd_with_overflow, this->i32Ty);
    this->_ssub32WO.init(llvm::Intrinsic::ssub_with_overflow, this->i32Ty);
    this->_smul32WO.init(llvm::Intrinsic::smul_with_overflow, this->i32Ty);
    this->_sadd64WO.init(llvm::Intrinsic::sadd_with_overflow, this->i64Ty);
    this->_ssub64WO.init(llvm::Intrinsic::ssub_with_overflow, this->i64Ty);
    this->_smul64WO.init(llvm::Intrinsic::smul_with_overflow, this->i64Ty);
    this->_ctlz32.init(llvm::Intrinsic::ctlz, this->i32Ty);
    this->_ctpop32.init(llvm::Intrinsic::ctpop, this->i32Ty);
    this->_cttz32.init(llvm::Intrinsic::cttz, this->i32Ty);
    this->_ctlz64.init(llvm::Intrinsic::ctlz, this->i64Ty);
    this->_ctpop64.init(llvm::Intrinsic::ctpop, this->i64Ty);
    this->_cttz64.init(llvm::Intrinsic::cttz, this->i64Ty);
    this->_fshl32.init(llvm::Intrinsic::fshl, this->i32Ty);
    this->_fshr32.init(llvm::Intrinsic::fshr, this->i32Ty);
    this->_fshl64.init(llvm::Intrinsic::fshl, this->i64Ty);
    this->_fshr64.init(llvm::Intrinsic::fshr, this->i64Ty);
    this->_fma32.init(llvm::Intrinsic::fma, this->f32Ty);
    this->_fabs32.init(llvm::Intrinsic::fabs, this->f32Ty);
    this->_sqrt32.init(llvm::Intrinsic::sqrt, this->f32Ty);
    this->_copysign32.init(llvm::Intrinsic::copysign, this->f32Ty);
    this->_fma64.init(llvm::Intrinsic::fma, this->f64Ty);
    this->_fabs64.init(llvm::Intrinsic::fabs, this->f64Ty);
    this->_sqrt64.init(llvm::Intrinsic::sqrt, this->f64Ty);
    this->_copysign64.init(llvm::Intrinsic::copysign, this->f64Ty);

    this->_readReg = nullptr;
    this->_spRegMD = nullptr;

} // constructor

void Context::beginModule (std::string_view src, int nClusters)
{
    this->_module = new llvm::Module (src, *this);

    this->_gen->beginModule (this->_module);

    // prepare the label-to-cluster map
    this->_clusterMap.clear();
    this->_clusterMap.reserve(nClusters);

    // reset the cached intrinsic functions
    this->_sadd32WO.reset();
    this->_ssub32WO.reset();
    this->_smul32WO.reset();
    this->_sadd64WO.reset();
    this->_ssub64WO.reset();
    this->_smul64WO.reset();
    this->_ctlz32.reset();
    this->_ctpop32.reset();
    this->_cttz32.reset();
    this->_ctlz64.reset();
    this->_ctpop64.reset();
    this->_cttz64.reset();
    this->_fshl32.reset();
    this->_fshr32.reset();
    this->_fshl64.reset();
    this->_fshr64.reset();
    this->_fma32.reset();
    this->_fabs32.reset();
    this->_sqrt32.reset();
    this->_copysign32.reset();
    this->_fma64.reset();
    this->_fabs64.reset();
    this->_sqrt64.reset();
    this->_copysign64.reset();

    this->_readReg = nullptr;
    this->_spRegMD = nullptr;

} // Context::beginModule

void Context::completeModule ()
{
}

void Context::optimize ()
{
    this->_gen->optimize (this->_module);
}

void Context::endModule ()
{
    this->_gen->endModule();
    delete this->_module;
}

void Context::beginCluster (CFG::cluster *cluster, llvm::Function *fn)
{
    assert ((cluster != nullptr) && "undefined cluster");
    assert ((fn != nullptr) && "undefined function");

    this->_overflowBB = nullptr;
    this->_overflowPhiNodes.clear();
    this->_fragMap.clear();
    this->_curFn = fn;
    this->_curCluster = cluster;

} // Context::beginCluster

void Context::endCluster ()
{
} // Context::endCluster

void Context::beginFrag ()
{
    this->_vMap.clear();

} // Context::beginFrag

llvm::Function *Context::newFunction (
    llvm::FunctionType *fnTy,
    std::string_view name,
    bool isPublic)
{
    llvm::Function *fn = llvm::Function::Create (
	    fnTy,
	    isPublic ? llvm::GlobalValue::ExternalLinkage : llvm::GlobalValue::PrivateLinkage,
	    name,
	    this->_module);

  // set the calling convention to our "Jump-with-arguments" convention
    fn->setCallingConv (llvm::CallingConv::JWA);

  // assign attributes to the function
    fn->addFnAttr (llvm::Attribute::NoUnwind);

    return fn;

}

// helper function to get the numbers of arguments/parameters for
// a fragment
Context::arg_info Context::_getArgInfo (frag_kind kind) const
{
    Context::arg_info info;

    info.nExtra = this->_regInfo.numMachineRegs();

    switch (kind) {
      case frag_kind::STD_FUN:
	info.nUnused = 0;
	info.basePtr = 0;
	break;
      case frag_kind::STD_CONT:
      // standard continuations do not use the first two registers of
      // the JWA convention (STD_LINK and STD_CLOS).
	info.nUnused = 2;
	info.basePtr = 0;
	break;
      case frag_kind::KNOWN_FUN:
      case frag_kind::INTERNAL:
        assert (this->_curCluster != nullptr && "no current cluster defined");
	info.nUnused = 0;
	if (this->_regInfo.usesBasePtr()
	&& this->_curCluster->get_attrs()->get_needsBasePtr()) {
	  // we need an extra argument for threading the base pointer
/* TODO: for KNOWN_FUN, we might have to pass the basePtr in memory! */
	    info.basePtr = 1;
	}
	else {
	    info.basePtr = 0;
	}
	break;
    }

    return info;

}

llvm::FunctionType *Context::createFnTy (frag_kind kind, Types_t const & tys) const
{
    Types_t allParams = this->createParamTys (kind, tys.size());

  // add the types from the function's formal parameters
    for (auto ty : tys) {
	allParams.push_back (ty);
    }

    return llvm::FunctionType::get (
	this->voidTy,
	llvm::ArrayRef<llvm::Type *>(allParams),
	false);

}

void Context::_addExtraParamTys (Types_t &tys, arg_info const &info) const
{
  // the parameter list starts with the special registers (i.e., alloc ptr, ...),
  //
    for (int i = 0;  i < info.nExtra;  ++i) {
	tys.push_back (this->ptrTy);
    }

    if (info.basePtr) {
	tys.push_back (this->intTy);
    }

}

Types_t Context::createParamTys (frag_kind kind, int n) const
{
    Types_t tys;
    arg_info info = this->_getArgInfo (kind);

    tys.reserve (info.numArgs(n));

  // the parameter list starts with the special registers (i.e., alloc ptr, ...),
  //
    this->_addExtraParamTys (tys, info);

  // we give the unused registers the ML value type
    for (int i = 0;  i < info.nUnused;  ++i) {
	tys.push_back (this->mlValueTy);
    }

    return tys;

}

void Context::_addExtraArgs (Args_t &args, arg_info const &info) const
{
  // seed the args array with the extra arguments
    for (int i = 0;  i < info.nExtra;  ++i) {
	args.push_back (this->_regState.get (this->_regInfo.machineReg(i)));
    }

    if (info.basePtr) {
	args.push_back (this->_regState.getBasePtr());
    }
}

Args_t Context::createArgs (frag_kind kind, int n)
{
    Args_t args;
    arg_info info = this->_getArgInfo (kind);

    args.reserve (info.numArgs(n));

    this->_addExtraArgs (args, info);

  // we assign the unused argument registers the undefined value
    for (int i = 0;  i < info.nUnused;  ++i) {
	args.push_back (llvm::UndefValue::get(this->mlValueTy));
    }

    return args;
}

// setup the incoming arguments for a standard function entry
//
void Context::setupStdEntry (CFG::attrs *attrs, CFG::frag *frag)
{
  // the order of incoming arguments is:
  //
  //	1. special registers: ALLOC_PTR, LIMIT_PTR, STORE_PTR, EXN_HNDLR, VAR_PTR
  //
  //    2. STD_LINK, STD_CLOS, STD_CONT
  //
  //	3. general purpose callee-saves (MISC0, MISC1, ...)
  //
  //	4. floating-point callee-saves (FPR0, FPR1, ...)
  //
  //    5. argument registers: STDARG, MISC{n}, MISC{n+1}, ... / FPR{m}, FPR{m+1}, ...
  //	   where "n" is the number of callee saves and "m" is the number of floating-point
  //	   callee-saves
  //
  // For continuations, the STD_LINK and STD_CLOS registers are undefined and do not
  // correspond to CFG parameters

    llvm::Function *fn = this->_curFn;

    arg_info info = this->_getArgInfo(frag->get_kind());

  // initialize the register state
    for (int i = 0, hwIx = 0;  i < CMRegInfo::NUM_REGS;  ++i) {
	CMRegInfo const *info = this->_regInfo.info(static_cast<CMRegId>(i));
	if (info->isMachineReg()) {
	    llvm::Argument *arg = this->_curFn->getArg(hwIx++);
#ifndef NDEBUG
	    arg->setName (info->name());
#endif
	    this->_regState.set (info->id(), arg);
	}
	else { // stack-allocated register
	    this->_regState.set (info->id(), nullptr);
	}
    }

  // initialize the base pointer (if necessary)
    if (this->_regInfo.usesBasePtr()
    && this->_curCluster->get_attrs()->get_needsBasePtr()) {
      // get the index of the register that holds the base address of the cluster
	int baseIx = (frag->get_kind() == frag_kind::STD_FUN)
	  // STDLINK holds the function's address and is the first non-special argument.
	    ? this->_regInfo.numMachineRegs()
	  // STDCONT holds the function's address and is the third non-special argument.
	    : this->_regInfo.numMachineRegs() + 2;
      // get base address of cluster and cast to the native int type
	auto basePtr = this->createPtrToInt (this->_curFn->getArg(baseIx));
	this->_regState.setBasePtr (basePtr);
#ifndef NDEBUG
	basePtr->setName ("basePtr");
#endif
    }
#ifndef NDEBUG
    else {
	this->_regState.clearBasePtr ();
    }
#endif

    std::vector<CFG::param *> params = frag->get_params();
    int baseIx = info.nExtra + info.nUnused;
    for (int i = 0;  i < params.size();  i++) {
	params[i]->bind (this, fn->getArg(baseIx + i));
    }

}

void Context::setupFragEntry (CFG::frag *frag, std::vector<llvm::PHINode *> &phiNodes)
{
    assert (frag->get_kind() == frag_kind::INTERNAL && "not an internal fragment");

    arg_info info = this->_getArgInfo (frag->get_kind());

  // initialize the register state
    for (int i = 0;  i < info.nExtra;  ++i) {
	CMRegInfo const *rInfo = this->_regInfo.machineReg(i);
	this->_regState.set (rInfo->id(), phiNodes[i]);
    }

    if (info.basePtr) {
      // we are using a base pointer
	this->_regState.setBasePtr (phiNodes[info.nExtra]);
    }

  // bind the formal parameters to the remaining PHI nodes
    std::vector<CFG::param *> params = frag->get_params();
    for (int i = 0;  i < params.size();  i++) {
	params[i]->bind (this, phiNodes[info.nExtra + info.basePtr + i]);
    }

} // Context::setupFragEntry

llvm::Constant *Context::createGlobalAlias (
    llvm::Type *ty,
    llvm::Twine const &name,
    llvm::Constant *v)
{
    auto alias = llvm::GlobalAlias::create (
	ty,
	0,
	llvm::GlobalValue::PrivateLinkage,
	name,
	v,
	this->_module);
    alias->setUnnamedAddr (llvm::GlobalValue::UnnamedAddr::Global);

    return alias;
}

llvm::Constant *Context::labelDiff (llvm::Function *f1, llvm::Function *f2)
{
  // define an alias for the value `(lab - curFn)`
    return this->createGlobalAlias (
	this->intTy,
	f1->getName() + "_sub_" + f2->getName(),
	llvm::ConstantExpr::getIntToPtr(
	    llvm::ConstantExpr::getSub (
		llvm::ConstantExpr::getPtrToInt(f1, this->intTy),
		llvm::ConstantExpr::getPtrToInt(f2, this->intTy)),
	    this->mlValueTy));

}

llvm::Constant *Context::blockDiff (llvm::BasicBlock *bb)
{
    return this->createGlobalAlias (
	this->intTy,
	bb->getName() + "_sub_" + this->_curFn->getName(),
	llvm::ConstantExpr::getIntToPtr(
	    llvm::ConstantExpr::getSub (
		llvm::ConstantExpr::getPtrToInt(this->blockAddr(bb), this->intTy),
		llvm::ConstantExpr::getPtrToInt(this->_curFn, this->intTy)),
	    this->mlValueTy));

}

llvm::Value *Context::evalLabel (llvm::Function *fn)
{
    if (this->_target->hasPCRel) {
#ifdef XXX
      // the target supports PC-relative addressing, so we can directly
      // refer to the function's label as a value.
	return fn;
#endif
      // the target supports PC-relative addressing, but we still need to
      // create an alias for `(lab - 0)` to force computation of the PC relative address.
	return this->createGlobalAlias (
	    this->intTy,
	    fn->getName() + "_alias",
	    llvm::ConstantExpr::getIntToPtr(
		llvm::ConstantExpr::getSub (
		    llvm::ConstantExpr::getPtrToInt(fn, this->intTy),
		    llvm::Constant::getNullValue(this->intTy)),
		this->mlValueTy));
    }
    else {
	llvm::Value *basePtr = this->_regState.getBasePtr();

	assert ((basePtr != nullptr) && "basePtr is not defined");

      // compute basePtr + (lab - curFn)
	auto labAdr = this->_builder.CreateIntToPtr(
	    this->createAdd (basePtr, this->labelDiff (fn, this->_curFn)),
	    this->mlValueTy);
#ifndef NDEBUG
	labAdr->setName ("L_" + fn->getName());
#endif
	return labAdr;
    }

} // Context::evalLabel

void Context::_initSPAccess ()
{
    assert ((this->_readReg == nullptr) && (this->_spRegMD == nullptr));
    this->_readReg = _getIntrinsic (llvm::Intrinsic::read_register, this->intTy);
    this->_spRegMD = llvm::MDNode::get (
	*this,
	llvm::MDString::get(*this, this->_target->spName));

}

// private function for loading a special register from memory
llvm::Value *Context::_loadMemReg (CMRegId r)
{
    auto info = this->_regInfo.info(r);
    return this->_loadFromStack (info->offset(), info->name());

} // Context::_loadMemReg

// private function for setting a special memory register
void Context::_storeMemReg (CMRegId r, llvm::Value *v)
{
    auto info = this->_regInfo.info(r);
    auto stkAddr = this->stkAddr (llvm::PointerType::get(*this, 0), info->offset());
    this->_builder.CreateAlignedStore (
	v,
	stkAddr,
	llvm::MaybeAlign (this->_wordSzB));

} // Context::_storeMemReg

// utility function for allocating a record of ML values (pointers or
// tagged ints).
//
llvm::Value *Context::allocRecord (llvm::Value *desc, Args_t const & args)
{
    assert (desc->getType() == this->intTy && "descriptor should be an integer");

    int len = args.size();
    llvm::Value *allocPtr = this->mlReg (CMRegId::ALLOC_PTR);

  // write object descriptor
    this->build().CreateAlignedStore (desc, allocPtr, llvm::MaybeAlign (this->_wordSzB));

  // initialize the object's fields
    for (int i = 1;  i <= len;  ++i) {
	this->build().CreateAlignedStore (
	    this->asMLValue (args[i-1]),
	    this->createGEP (allocPtr, i),
	    llvm::MaybeAlign (this->_wordSzB));
    }

  // compute the object's address and cast it to an ML value
    llvm::Value *obj = this->asMLValue (this->createGEP (allocPtr, 1));

  // bump the allocation pointer
    this->setMLReg (CMRegId::ALLOC_PTR, this->createGEP (allocPtr, len + 1));

    return obj;
}

void Context::callGC (
    Args_t const & roots,
    std::vector<LambdaVar::lvar> const & newRoots)
{
    assert ((this->_gcFnTy->getNumParams() == roots.size())
	&& "arity mismatch in GC call");

  // get the address of the "call-gc" entry
    llvm::Value *callGCFn = this->_loadFromStack (this->_target->callGCOffset, "callGC");

  // call the garbage collector.  The return type of the GC is a struct
  // that contains the post-GC values of the argument registers
    auto call = this->_builder.CreateCall (
	this->_gcFnTy,
	this->createBitCast(callGCFn, llvm::PointerType::get(*this, 0)),
	roots);
    call->setCallingConv (llvm::CallingConv::JWA);
    call->setTailCallKind (llvm::CallInst::TCK_NoTail);

  // restore the register state from the return struct
    for (unsigned i = 0, hwIx = 0;  i < CMRegInfo::NUM_REGS;  ++i) {
	CMRegInfo const *info = this->_regInfo.info(static_cast<CMRegId>(i));
	if (info->isMachineReg()) {
	    auto reg = this->_builder.CreateExtractValue(call, { hwIx });
	    this->setMLReg (info->id(), reg);
	    hwIx++;
	}
    }

  // extract the new roots from the return struct
    unsigned ix = this->_regInfo.numMachineRegs();
    for (auto lv : newRoots) {
	this->insertVal (lv, this->_builder.CreateExtractValue(call, { ix++ }));
    }

} // Context::callGC

// return branch-weight meta data, where `prob` represents the probability of
// the true branch and is in the range 1..999.
llvm::MDNode *Context::branchProb (int prob)
{
    auto name = llvm::MDString::get(*this, "branch_weights");
    auto trueProb = llvm::ValueAsMetadata::get(this->i32Const(prob));
    auto falseProb = llvm::ValueAsMetadata::get(this->i32Const(1000 - prob));
    auto tpl = llvm::MDTuple::get(*this, {name, trueProb, falseProb});

    return tpl;

} // Context::branchProb

// generate a type cast for an actual to formal transfer.
//
llvm::Value *Context::castTy (llvm::Type *srcTy, llvm::Type *tgtTy, llvm::Value *v)
{
    if (tgtTy->isPointerTy()) {
	if (srcTy->isPointerTy()) {
	    return this->_builder.CreateBitCast(v, tgtTy);
	} else {
	    return this->_builder.CreateIntToPtr(v, tgtTy);
	}
    }
    else if (tgtTy->isIntegerTy() && srcTy->isPointerTy()) {
	return this->_builder.CreatePtrToInt(v, tgtTy);
    }
    else {
	assert (false && "invalid type cast");
	return nullptr;
    }

} // Context::castTy

llvm::Function *Context::_getIntrinsic (llvm::Intrinsic::ID id, llvm::Type *ty) const
{
    return llvm::Intrinsic::getOrInsertDeclaration (
	this->_module, id, llvm::ArrayRef<llvm::Type *>(ty));
}

std::unique_ptr<ObjectFile> Context::compile () const
{
    return this->_gen->emitMC (this->_module);
}

void Context::dumpAsm () const
{
    this->_gen->emitFile (this->_module, "-", llvm::CodeGenFileType::AssemblyFile);
}

void Context::dumpAsm (std::string_view stem) const
{
    std::string outFile = std::string(stem) + ".s";
    this->_gen->emitFile (this->_module, outFile, llvm::CodeGenFileType::AssemblyFile);
}

void Context::dumpObj (std::string_view stem) const
{
// FIXME: use OBJECT_FILE_EXTENSION so that on Windows the extension is ".obj"
    std::string outFile = std::string(stem) + ".o";
    this->_gen->emitFile (this->_module, outFile, llvm::CodeGenFileType::ObjectFile);
}

void Context::dumpLL (std::string_view stem) const
{
    std::error_code EC;
    std::string outFile = std::string(stem) + ".ll";
    llvm::raw_fd_ostream outS(outFile, EC, llvm::sys::fs::OF_Text);
    if (EC) {
/* FIXME: report the error */
    }
    this->_module->print(outS, nullptr);
    outS.close();

}

// dump the current module to stderr
void Context::dump () const
{
    this->_module->dump();
}

// run the LLVM verifier on the module
bool Context::verify () const
{
    return llvm::verifyModule (*this->_module, &llvm::dbgs());
}

} // namespace cfgcg
} // namespace smlnj
