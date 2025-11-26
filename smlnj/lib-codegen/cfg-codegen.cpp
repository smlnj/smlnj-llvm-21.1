///! \file cfg-codegen.cpp
///
/// \copyright 2023 The Fellowship of SML/NJ (https://smlnj.org)
/// All rights reserved.
///
/// \brief This file holds the implementations of the `codegen` methods
/// for the CFG expression and statement types (defined in the `CFG` module).
///
/// \author John Reppy
///

#include "cfg.hpp"
#include "target-info.hpp"

namespace CFG {

  /***** code generation for the `ty` type *****/

    llvm::Type *LABt::codegen (smlnj::cfgcg::Context *cxt)
    {
	return cxt->mlValueTy;

    } // LABt::codegen

    llvm::Type *PTRt::codegen (smlnj::cfgcg::Context *cxt)
    {
	return cxt->mlValueTy;

    } // PTRt::codegen

    llvm::Type *TAGt::codegen (smlnj::cfgcg::Context *cxt)
    {
	return cxt->intTy;

    } // TAGt::codegen

    llvm::Type *NUMt::codegen (smlnj::cfgcg::Context *cxt)
    {
	return cxt->iType (this->_v_sz);

    } // NUMt::codegen

    llvm::Type *FLTt::codegen (smlnj::cfgcg::Context *cxt)
    {
	return cxt->fType (this->_v_sz);

    } // FLTt::codegen

  // code generation for a vector of types
    static Types_t genTypes (smlnj::cfgcg::Context *cxt, std::vector<ty *> const &tys)
    {
	Types_t llvmTys;
	llvmTys.reserve(tys.size());
	for (auto ty : tys) {
	    llvmTys.push_back (ty->codegen (cxt));
	}
	return llvmTys;
    }

  /***** code generation for the `exp` type *****/

    llvm::Value *VAR::codegen (smlnj::cfgcg::Context *cxt)
    {
	llvm::Value *v = cxt->lookupVal (this->_v_name);
#ifndef NDEBUG
	if (v == nullptr) {
	    llvm::dbgs() << "VAR: " << this->_v_name << " is unbound\n";
	    assert (v && "unbound variable");
	}
#endif
	return v;

    } // VAR::codegen

    llvm::Value *LABEL::codegen (smlnj::cfgcg::Context *cxt)
    {
	cluster *cluster = cxt->lookupCluster (this->_v_name);

	assert (cluster && "Unknown cluster label");

	return cxt->evalLabel (cluster->fn());

    } // LABEL::codegen

    llvm::Value *NUM::codegen (smlnj::cfgcg::Context *cxt)
    {
	if (this->get_iv().getSign() < 0) {
	    return cxt->iConst (this->get_sz(), this->get_iv().toInt64());
	} else {
	    return cxt->uConst (this->get_sz(), this->get_iv().toUInt64());
	}

    } // NUM::codegen

    llvm::Value *LOOKER::codegen (smlnj::cfgcg::Context *cxt)
    {
	Args_t args;
	for (auto it = this->_v_args.begin(); it != this->_v_args.end(); ++it) {
	    args.push_back ((*it)->codegen (cxt));
	}
	return this->_v_oper->codegen (cxt, args);

    } // LOOKER::codegen

    llvm::Value *PURE::codegen (smlnj::cfgcg::Context *cxt)
    {
	Args_t args;
	for (auto it = this->_v_args.begin(); it != this->_v_args.end(); ++it) {
	    args.push_back ((*it)->codegen (cxt));
	}
	return this->_v_oper->codegen (cxt, args);

    } // PURE::codegen

    llvm::Value *SELECT::codegen (smlnj::cfgcg::Context *cxt)
    {
	llvm::Value *adr = cxt->createGEP (
	    cxt->asPtr(this->_v_arg->codegen(cxt)),
	    static_cast<int32_t>(this->_v_idx));
	return cxt->setInvarianteLoadMD (cxt->createLoad (cxt->mlValueTy, adr));

    } // SELECT::codegen

  /***** code generation for the `stm` type *****/

    void LET::codegen (smlnj::cfgcg::Context *cxt)
    {
      // record mapping from the parameter to the compiled expression
	this->_v1->bind (cxt, this->_v0->codegen(cxt));
      // compile continuation
	this->_v2->codegen(cxt);

    } // LET::codegen

    void ALLOC::codegen (smlnj::cfgcg::Context *cxt)
    {
	Args_t args;
	for (auto it = this->_v1.begin(); it != this->_v1.end(); ++it) {
	    args.push_back ((*it)->codegen (cxt));
	}
	cxt->insertVal (this->_v2, this->_v0->codegen(cxt, args));

      // compile continuation
	this->_v3->codegen(cxt);

    } // ALLOC::codegen

  // helper function for argument set up for APPLY and THROW
    inline Args_t SetupStdArgs (
	smlnj::cfgcg::Context *cxt,
	llvm::FunctionType *fnTy,
	frag_kind fk,
	std::vector<exp *> const &cfgArgs)
    {
	Args_t args = cxt->createArgs (fk, cfgArgs.size());

	int base = args.size(); // index of first user parameter
	for (int i = 0;  i < cfgArgs.size();  ++i) {
	    auto arg = cfgArgs[i]->codegen (cxt);
	    auto paramTy = fnTy->getParamType(base + i);
	    if (paramTy != arg->getType()) {
		arg = cxt->castTy(arg->getType(), paramTy, arg);
	    }
	    args.push_back (arg);
	}

	return args;

    } // SetupStdArgs

    void APPLY::codegen (smlnj::cfgcg::Context *cxt)
    {
	frag_kind fk = frag_kind::STD_FUN;
	llvm::FunctionType *fnTy;
	llvm::Value *fn;
	LABEL *lab = (this->_v0->isLABEL() ? reinterpret_cast<LABEL *>(this->_v0) : nullptr);
	if (lab == nullptr) {
	    fnTy = cxt->createFnTy (fk, genTypes (cxt, this->_v2));
	    fn = cxt->build().CreateBitCast(
		this->_v0->codegen (cxt),
		fnTy->getPointerTo());
	} else {
	    cluster *f = cxt->lookupCluster (lab->get_name());
	    assert (f && "APPLY of unknown cluster");
	    fk = f->entry()->get_kind();
	    fn = f->fn();
	    fnTy = f->fn()->getFunctionType();
	}

#ifndef NDEBUG
        // check that we do not exceed the maximum number of argument registers
        int nGP = cxt->targetInfo()->numGPArgRegs;
        int nFP = cxt->targetInfo()->numFPArgRegs;
        for (auto ty : this->_v2) {
            if (ty->isFLTt()) { --nFP; } else { --nGP; }
        }
        if ((nGP < 0) || (nFP < 0)) {
	    llvm::dbgs() << "APPLY ";
            if (lab == nullptr) {
                auto f = reinterpret_cast<VAR *>(this->_v0);
                llvm::dbgs() << "v" << f->get_name();
            } else {
                llvm::dbgs() << lab->get_name();
            }
            llvm::dbgs() << ": "
                << (cxt->targetInfo()->numGPArgRegs - nGP) << "/"
                << cxt->targetInfo()->numGPArgRegs << " GP args; "
                << (cxt->targetInfo()->numFPArgRegs - nFP) << "/"
                << cxt->targetInfo()->numFPArgRegs << " FP args\n";
	    assert (false && "too many arguments");
        }
#endif

        // evaluate the arguments
	Args_t args = SetupStdArgs (cxt, fnTy, fk, this->_v1);

	cxt->createJWACall(fnTy, fn, args);

	cxt->build().CreateRetVoid();

    } // APPLY::codegen

    void THROW::codegen (smlnj::cfgcg::Context *cxt)
    {
	llvm::FunctionType *fnTy;
	llvm::Value *fn;
	LABEL *lab = (this->_v0->isLABEL() ? reinterpret_cast<LABEL *>(this->_v0) : nullptr);
	if (lab == nullptr) {
	    fnTy = cxt->createFnTy (frag_kind::STD_CONT, genTypes (cxt, this->_v2));
	    fn = cxt->build().CreateBitCast(
		this->_v0->codegen (cxt),
		fnTy->getPointerTo());
	} else {
	    cluster *f = cxt->lookupCluster (lab->get_name());
	    assert (f && "THROW of unknown cluster");
	    fn = f->fn();
	    fnTy = f->fn()->getFunctionType();
	}

#ifndef NDEBUG
        // check that we do not exceed the maximum number of argument registers
        int nGP = cxt->targetInfo()->numGPArgRegs - 2; /* two unused registers */
        int nFP = cxt->targetInfo()->numFPArgRegs;
        for (auto ty : this->_v2) {
            if (ty->isFLTt()) { --nFP; } else { --nGP; }
        }
        if ((nGP < 0) || (nFP < 0)) {
	    llvm::dbgs() << "THROW ";
            if (lab == nullptr) {
                auto f = reinterpret_cast<VAR *>(this->_v0);
                llvm::dbgs() << "v" << f->get_name();
            } else {
                llvm::dbgs() << lab->get_name();
            }
            llvm::dbgs() << ": "
                << (cxt->targetInfo()->numGPArgRegs - 2 - nGP) << "/"
                << (cxt->targetInfo()->numGPArgRegs - 2) << " GP args; "
                << (cxt->targetInfo()->numFPArgRegs - nFP) << "/"
                << cxt->targetInfo()->numFPArgRegs << " FP args\n";
	    assert (false && "too many arguments");
        }
#endif

      // evaluate the arguments
	Args_t args = SetupStdArgs (cxt, fnTy, frag_kind::STD_CONT, this->_v1);

	cxt->createJWACall(fnTy, fn, args);

	cxt->build().CreateRetVoid();

    } // THROW::codegen

    void GOTO::codegen (smlnj::cfgcg::Context *cxt)
    {
	llvm::BasicBlock *srcBB = cxt->getCurBB();
	frag *dstFrag = cxt->lookupFrag (this->_v0);

	assert (dstFrag && (dstFrag->get_kind() == frag_kind::INTERNAL));

      // evaluate the arguments
	Args_t args = cxt->createArgs (frag_kind::INTERNAL, this->_v1.size());
	for (auto arg : this->_v1) {
	    args.push_back (arg->codegen (cxt));
	}

      // add outgoing values as incoming values to the destination's
      // phi nodes
	for (int i = 0;  i < args.size();  ++i) {
	  // make sure that the type match!
	    assert (args[i]);
	    llvm::Type *srcTy = args[i]->getType();
	    llvm::Type *tgtTy = dstFrag->paramTy(i);
	    if (srcTy != tgtTy) {
		dstFrag->addIncoming (i, cxt->castTy(srcTy, tgtTy, args[i]), srcBB);
	    } else {
		dstFrag->addIncoming (i, args[i], srcBB);
	    }
	}

      // generate the control transfer; note that we need to do this *after*
      // updating the PHI nodes, since any type casts introduced for the PHI
      // nodes will be generated in the *source* block!
	cxt->createBr (dstFrag->bb());

    } // GOTO::codegen

    void SWITCH::codegen (smlnj::cfgcg::Context *cxt)
    {
      // evaluate the argument and truncate to 32 bits
	llvm::Value *arg = cxt->createTrunc(cxt->asInt(this->_v0->codegen(cxt)), cxt->i32Ty);

      // the number of non-default cases
	int nCases = this->_v1.size();

      // save the current block
	auto curBlk = cxt->getCurBB();

      // as suggested by Matthew Fluet, we mark the default case as unreachable,
      // which has the effect of getting LLVM to treat the switch as being exhaustive.
	auto dfltBlk = cxt->newBB("impossible");
	cxt->setInsertPoint (dfltBlk);
	cxt->build().CreateUnreachable();

      // create the switch in the current block
	cxt->setInsertPoint (curBlk);
	llvm::SwitchInst *sw = cxt->build().CreateSwitch(arg, dfltBlk, nCases);

      // add the cases to the switch
	for (int i = 0;  i < nCases;  i++) {
	    sw->addCase (cxt->iConst(32, i), this->_v1[i]->bb());
	}

      // generate the code for the basic blocks
	smlnj::cfgcg::CMRegState saveRegs;
	cxt->saveSMLRegState (saveRegs);
	for (auto it = this->_v1.begin();  it != this->_v1.end();  ++it) {
	    cxt->restoreSMLRegState (saveRegs);
	    cxt->setInsertPoint ((*it)->bb());
	    (*it)->codegen (cxt);
	}

    } // SWITCH::codegen

    void BRANCH::codegen (smlnj::cfgcg::Context *cxt)
    {
      // evaluate the test
	Args_t args;
	for (auto it = this->_v1.begin(); it != this->_v1.end(); ++it) {
	    args.push_back ((*it)->codegen (cxt));
	}
	llvm::Value *cond = this->_v0->codegen(cxt, args);

      // generate the conditional branch
	if (this->_v2 == 0) {
	  // no branch prediction
	    cxt->build().CreateCondBr(cond, this->_v3->bb(), this->_v4->bb());
	} else {
	    cxt->build().CreateCondBr(
		cond,
		this->_v3->bb(), this->_v4->bb(),
		cxt->branchProb (this->_v2));
	}

      // generate code for the true branch
	smlnj::cfgcg::CMRegState saveRegs;
	cxt->saveSMLRegState (saveRegs);
	cxt->setInsertPoint (this->_v3->bb());
	this->_v3->codegen (cxt);

      // generate code for the false branch
	cxt->restoreSMLRegState (saveRegs);
	cxt->setInsertPoint (this->_v4->bb());
	this->_v4->codegen (cxt);

    } // BRANCH::codegen

    void ARITH::codegen (smlnj::cfgcg::Context *cxt)
    {
	Args_t args;
	for (auto it = this->_v1.begin(); it != this->_v1.end(); ++it) {
	    args.push_back ((*it)->codegen (cxt));
	}
      // record mapping from the parameter to the compiled expression
	this->_v2->bind (cxt, this->_v0->codegen (cxt, args));
      // compile continuation
	this->_v3->codegen (cxt);

    } // ARITH::codegen

    void SETTER::codegen (smlnj::cfgcg::Context *cxt)
    {
	Args_t args;
	for (auto it = this->_v1.begin(); it != this->_v1.end(); ++it) {
	    args.push_back ((*it)->codegen (cxt));
	}
	this->_v0->codegen (cxt, args);
      // compile continuation
	this->_v2->codegen (cxt);

    } // SETTER::codegen

    void CALLGC::codegen (smlnj::cfgcg::Context *cxt)
    {
      // evaluate the roots
	Args_t roots = cxt->createArgs (frag_kind::STD_FUN, this->_v0.size());
	for (auto it = this->_v0.begin(); it != this->_v0.end(); ++it) {
	  // all roots are SML values, so make sure they have the correct type
	    roots.push_back (cxt->asMLValue ((*it)->codegen (cxt)));
	}

	cxt->callGC (roots, this->_v1);

      // compile continuation
	this->_v2->codegen (cxt);

    } // CALLGC::codegen

    void RCC::codegen (smlnj::cfgcg::Context *cxt)
    {
	assert (false && "RCC not yet implemented"); /* TODO */
    } // RCC::codegen


  /***** code generation for the `frag` type *****/

    void frag::codegen (smlnj::cfgcg::Context *cxt, cluster *cluster)
    {
	cxt->beginFrag ();

	cxt->setInsertPoint (this->_v_body->bb());

	if (cluster != nullptr) {
	    cxt->setupStdEntry (cluster->get_attrs(), this);
	} else {
	    cxt->setupFragEntry (this, this->_phiNodes);
	}

      // generate code for the fragment
	this->_v_body->codegen (cxt);

    } // frag::codegen


  /***** code generation for the `cluster` type *****/

    void cluster::codegen (smlnj::cfgcg::Context *cxt, bool isFirst)
    {
	cxt->beginCluster (this, this->_fn);

      // initialize the fragments for the cluster
	for (auto frag : this->_v_frags) {
	    frag->init (cxt);
	}

      // generate code for the cluster
	this->_v_frags[0]->codegen (cxt, this);
	for (int i = 1;  i < this->_v_frags.size();  ++i) {
	    this->_v_frags[i]->codegen (cxt, nullptr);
	}

	cxt->endCluster ();

    } // cluster::codegen


  /***** code generation for the `comp_unit` type *****/

    void comp_unit::codegen (smlnj::cfgcg::Context *cxt)
    {
      // initialize the buffer for the comp_unit
	cxt->beginModule (this->_v_srcFile, this->_v_fns.size() + 1);

      // initialize the clusters
	this->_v_entry->init (cxt, true);
	for (auto f : this->_v_fns) {
	    f->init (cxt, false);
	}

      // generate code
	this->_v_entry->codegen (cxt, true);
	for (auto f : this->_v_fns) {
	    f->codegen (cxt, false);
	}

        cxt->completeModule ();

    } // comp_unit::codegen

} // namespace CFG
