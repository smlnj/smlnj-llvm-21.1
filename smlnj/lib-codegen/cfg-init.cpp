/// \file cfg-init.cpp
///
/// \copyright 2023 The Fellowship of SML/NJ (https://smlnj.org)
/// All rights reserved.
///
/// \brief This file holds the implementation of the `init` methods
/// for the various CFG types (defined in the `CFG` module).
///
/// \author John Reppy
///

#include "cfg.hpp"

namespace CFG {

  /***** initialization for the `stm` type *****/

  // helper function for setting the `_bb` field of a `stm`
  //
    inline void stm::_initBB (smlnj::cfgcg::Context *cxt, bool blkEntry)
    {
	if (blkEntry) {
	    this->_bb = cxt->newBB();
	} else {
	    this->_bb = nullptr;
	}
    }

    void LET::init (smlnj::cfgcg::Context *cxt, bool blkEntry)
    {
	this->_initBB (cxt, blkEntry);

      // continue initialization
	this->_v2->init (cxt, false);

    } // LET::init

    void ALLOC::init (smlnj::cfgcg::Context *cxt, bool blkEntry)
    {
	this->_initBB (cxt, blkEntry);

      // continue initialization
	this->_v3->init (cxt, false);

    } // ALLOC::init

    void APPLY::init (smlnj::cfgcg::Context *cxt, bool blkEntry)
    {
	this->_initBB (cxt, blkEntry);

    } // APPLY::init

    void THROW::init (smlnj::cfgcg::Context *cxt, bool blkEntry)
    {
	this->_initBB (cxt, blkEntry);

    } // THROW::init

    void GOTO::init (smlnj::cfgcg::Context *cxt, bool blkEntry)
    {
	this->_initBB (cxt, blkEntry);

    } // GOTO::init

    void SWITCH::init (smlnj::cfgcg::Context *cxt, bool blkEntry)
    {
	this->_initBB (cxt, blkEntry);

      // initialize arms of switch
	for (auto it = this->_v1.begin();  it != this->_v1.end();  ++it) {
	    (*it)->init (cxt, true);
	}

    } // SWITCH::init

    void BRANCH::init (smlnj::cfgcg::Context *cxt, bool blkEntry)
    {
	this->_initBB (cxt, blkEntry);

      // initialize arms of conditional
	this->_v3->init (cxt, true);
	this->_v4->init (cxt, true);

    } // BRANCH::init

    void ARITH::init (smlnj::cfgcg::Context *cxt, bool blkEntry)
    {
	this->_initBB (cxt, blkEntry);

      // continue initialization
	this->_v3->init (cxt, false);

    } // ARITH::init

    void SETTER::init (smlnj::cfgcg::Context *cxt, bool blkEntry)
    {
	this->_initBB (cxt, blkEntry);

      // continue initialization
	this->_v2->init (cxt, false);

    } // SETTER::init

    void CALLGC::init (smlnj::cfgcg::Context *cxt, bool blkEntry)
    {
	this->_initBB (cxt, blkEntry);

      // continue initialization
	this->_v2->init (cxt, false);

    } // CALLGC::init

    void RCC::init (smlnj::cfgcg::Context *cxt, bool blkEntry)
    {
	this->_initBB (cxt, blkEntry);

      // continue initialization
	this->_v_k->init (cxt, false);

    } // RCC::init


  /***** initialization for the `frag` type *****/

    // for each fragment in the cluster, we add the mapping from the fragment's
    // label to it; we also initialize the fragment's body (which creates the
    // entry block for the fragment) and we add phi nodes to the block for each
    // of the parameters.
    //
    void frag::init (smlnj::cfgcg::Context *cxt)
    {
      // add the fragment to the label to fragment map
	cxt->insertFrag (this->_v_lab, this);

      // initialize the fragment's body */
	this->_v_body->init (cxt, true);
#ifndef NDEBUG
	this->_v_body->bb()->setName ("L_" + std::to_string(this->_v_lab));
#endif

      // add a phi node for each parameter of the fragment
	if (this->_v_kind == frag_kind::INTERNAL) {
	  // compute the parameter types for the fragment
	    auto paramTys = cxt->createParamTys (this->_v_kind, this->_v_params.size());
	    for (auto param : this->_v_params) {
		paramTys.push_back (param->get_ty()->codegen (cxt));
	    }
	  // for each parameter, add a PHI node to the entry block
	    cxt->setInsertPoint (this->_v_body->bb());
	    this->_phiNodes.reserve (paramTys.size());
	    for (auto ty : paramTys) {
		llvm::PHINode *phi = cxt->build().CreatePHI(ty, 0);
		this->_phiNodes.push_back (phi);
	    }
	}

    }


  /***** initialization for the `cluster` type *****/

    void cluster::init (smlnj::cfgcg::Context *cxt, bool isEntry)
    {
	assert (this->_v_frags.size() > 0);

	frag *entry = this->_v_frags[0];

      // add the cluster to the cluster map
	cxt->insertCluster (entry->get_lab(), this);

      // set the current cluster in the code buffer (needed to allow access to the
      // cluster's attributes)
	cxt->setCluster (this);

      // create and record the LLVM function for the cluster
	auto params = entry->get_params();
	std::vector<llvm::Type *> paramTys;
	paramTys.reserve (params.size());
	for (auto param : params) {
	    paramTys.push_back (param->get_ty()->codegen(cxt));
	}
	llvm::FunctionType *fnTy = cxt->createFnTy (entry->get_kind(), paramTys);

	std::string name = (isEntry ? "entry" : "fn") + std::to_string(entry->get_lab());
	this->_fn = cxt->newFunction (fnTy, name, isEntry);

    }

} // namespace CFG
