/// \file mc-gen.cpp
///
/// \copyright 2023 The Fellowship of SML/NJ (https://smlnj.org)
/// All rights reserved.
///
/// \brief Wrapper for the low-level machine-specific parts of the code generator
///
/// \author John Reppy
///

#include "objfile-stream.hpp"
#include "target-info.hpp"
#include "mc-gen.hpp"
#include "context.hpp"
#include "object-file.hpp"

#include "llvm/MC/TargetRegistry.h"
#include "llvm/IR/LegacyPassManager.h" /* needed for code gen */
#include "llvm/Passes/OptimizationLevel.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar/DCE.h"
#include "llvm/Transforms/Scalar/DivRemPairs.h"
#include "llvm/Transforms/Scalar/EarlyCSE.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Scalar/LowerExpectIntrinsic.h"
#include "llvm/Transforms/Scalar/Reassociate.h"
#include "llvm/Transforms/Scalar/SCCP.h"
#include "llvm/Transforms/Scalar/SimplifyCFG.h"
#include "llvm/Support/SmallVectorMemoryBuffer.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/Support/FileSystem.h"

#include <iostream>

namespace smlnj {
namespace cfgcg {

MCGen::MCGen (TargetInfo const *info)
  : _tgtInfo(info), _tgtMachine(nullptr), _mam(), _fam(), _pb(nullptr)
{
  // get the LLVM target triple
    llvm::Triple triple = info->getTriple();

  // lookup the target in the registry using the triple's string representation
    std::string errMsg;
    auto *target = llvm::TargetRegistry::lookupTarget(triple.str(), errMsg);
    if (target == nullptr) {
	std::cerr << "**** Fatal error: unable to find target for \""
	    << info->name << "\"\n";
	std::cerr << "    [" << errMsg << "]\n";
        assert(false);
    }

    llvm::TargetOptions tgtOptions;

  // floating-point target options
    tgtOptions.setFP32DenormalMode (llvm::DenormalMode::getIEEE());
    tgtOptions.setFPDenormalMode (llvm::DenormalMode::getIEEE());

// TODO: enable tgtOptions.EnableFastISel?

  // make sure that tail calls are optimized
  /* It turns out that setting the GuaranteedTailCallOpt flag to true causes
   * a bug with non-tail JWA calls (the bug is a bogus stack adjustment after
   * the call).  Fortunately, our tail calls get properly optimized even
   * without that flag being set.
   */
//    tgtOpts.GuaranteedTailCallOpt = true;

// see include/llvm/Support/*Parser.def for the various CPU and feature names
// that are recognized
    std::unique_ptr<llvm::TargetMachine> tgtMachine(target->createTargetMachine(
	triple.str(),
	"generic",		/* CPU name */
	"",			/* features string */
	tgtOptions,
	llvm::Reloc::PIC_,
	std::optional<llvm::CodeModel::Model>(),
	llvm::CodeGenOptLevel::Less));

    if (!tgtMachine) {
	std::cerr << "**** Fatal error: unable to create target machine\n";
        assert(false);
    }

    this->_tgtMachine = std::move(tgtMachine);

    // Create the new pass manager builder.
    this->_pb = new llvm::PassBuilder(this->_tgtMachine.get());

    // we only perform function-level optimizations, so we only need
    // the function analysis
    llvm::LoopAnalysisManager lam;
    llvm::CGSCCAnalysisManager cgam;
    this->_pb->registerModuleAnalyses(this->_mam);
    this->_pb->registerCGSCCAnalyses(cgam);
    this->_pb->registerFunctionAnalyses(this->_fam);
    this->_pb->registerLoopAnalyses(lam);
    this->_pb->crossRegisterProxies(lam, this->_fam, cgam, this->_mam);

    // set up the optimization passes
    llvm::FunctionPassManager fpm;
    fpm.addPass(llvm::LowerExpectIntrinsicPass());      // -lower-expect
    fpm.addPass(llvm::SimplifyCFGPass());               // -simplifycfg
    fpm.addPass(llvm::InstCombinePass());               // -instcombine
    fpm.addPass(llvm::ReassociatePass());               // -reassociate
    fpm.addPass(llvm::EarlyCSEPass(false));             // -early-cse
    fpm.addPass(llvm::GVNPass());                       // -gvn
    fpm.addPass(llvm::SCCPPass());                      // -sccp
    fpm.addPass(llvm::DCEPass());                       // -dce
    fpm.addPass(llvm::SimplifyCFGPass());               // -simplifycfg
    fpm.addPass(llvm::InstCombinePass());               // -instcombine
    // for the last simplification pass, we want to convert switches to jump tables
    llvm::SimplifyCFGOptions opts;
    opts.ConvertSwitchToLookupTable = true;
    fpm.addPass(llvm::SimplifyCFGPass(opts));           // -simplifycfg

    this->_pm.addPass (llvm::createModuleToFunctionPassAdaptor(std::move(fpm)));

} // MCGen constructor

MCGen::~MCGen ()
{
    if (this->_pb != nullptr) {
        delete this->_pb;
    }

}

void MCGen::beginModule (llvm::Module *module)
{
  // tell the module about the target machine
    module->setTargetTriple(this->_tgtMachine->getTargetTriple().getTriple());
    module->setDataLayout(this->_tgtMachine->createDataLayout());

} // MCGen::beginModule

void MCGen::endModule () { }

void MCGen::optimize (llvm::Module *module)
{
  // run the function optimizations over every function
    this->_pm.run (*module, this->_mam);

}

// compile the code into the code buffer's object-file backing store.
//
// adopted from SimpleCompiler::operator() (lib/ExecutionEngine/Orc/CompileUtils.cpp)
//
std::unique_ptr<ObjectFile> MCGen::emitMC (llvm::Module *module)
{
    ObjfileStream objStrm;
    llvm::legacy::PassManager pass;
    llvm::MCContext *ctx; /* result parameter */
    if (this->_tgtMachine->addPassesToEmitMC(pass, ctx, objStrm)) {
        llvm::report_fatal_error ("unable to add pass to generate code", true);
    }
    pass.run (*module);

    return std::make_unique<ObjectFile>(objStrm);

}

void MCGen::emitFile (
    llvm::Module *module,
    std::string const &outFile,
    llvm::CodeGenFileType fileType)
{
    std::error_code EC;
    llvm::raw_fd_ostream outStrm(outFile, EC, llvm::sys::fs::OF_None);
    if (EC) {
        llvm::errs() << "unable to open output file '" << outFile << "'\n";
        return;
    }

    llvm::legacy::PassManager pass;
    if (this->_tgtMachine->addPassesToEmitFile(pass, outStrm, nullptr, fileType)) {
        llvm::errs() << "unable to add pass to generate '" << outFile << "'\n";
        return;
    }

    pass.run(*module);

    outStrm.flush();

}

} // namespace cfgcg
} // namespace smlnj
