///! \file cfg-prim-codegen.cpp
///
/// \copyright 2025 The Fellowship of SML/NJ (https://smlnj.org)
/// All rights reserved.
///
/// \brief This file holds the implementations of the `codegen` methods
/// for the CFG primitive operations (defined in the `CFG_Prim` module).
///
/// \author John Reppy
///

#include "cfg.hpp"
#include "target-info.hpp"

namespace CFG_Prim {

  // helper function to get LLVM type for numbers
    inline llvm::Type *numType (smlnj::cfgcg::Context *cxt, numkind k, int sz)
    {
        return (k == numkind::INT ? cxt->iType(sz) : cxt->fType(sz));
    }

  // helper function to convert bit size to byte size
    inline unsigned bitsToBytes (unsigned n) { return (n >> 3); }

  // helper function to ensure that arguments to arithmetic operations
  // have an LLVM integer type, since we use i64* (or i32*) as the type
  // of ML values
    inline llvm::Value *castArgToInt (smlnj::cfgcg::Context *cxt, unsigned sz, llvm::Value *arg)
    {
        if (arg->getType() == cxt->mlValueTy) {
            return cxt->build().CreatePtrToInt(arg, cxt->iType(sz));
        } else {
            return arg;
        }
    }

  /***** code generation for the `alloc` type *****/
    llvm::Value *SPECIAL::codegen (smlnj::cfgcg::Context *cxt, Args_t const &args)
    {
      // the first argument is the record's descriptor word and the second
      // is the content of the special object.
        assert (args.size() == 2 && "expected descriptor and content");
        return cxt->allocRecord (cxt->asInt(args[0]), { args[1] });

    }

    llvm::Value *RECORD::codegen (smlnj::cfgcg::Context *cxt, Args_t const &args)
    {
        return cxt->allocRecord (cxt->uConst(this->_v_desc.toUInt64()), args);

    } // RECORD::codegen

    llvm::Value *RAW_RECORD::codegen (smlnj::cfgcg::Context *cxt, Args_t const &args)
    {
        int len = args.size();
        llvm::Value *allocPtr = cxt->mlReg (smlnj::cfgcg::CMRegId::ALLOC_PTR);

        assert (len == this->_v_fields.size() && "incorrect number of fields");

/* FIXME: for 32-bit targets, we may need to align the allocation pointer; to do
 * this properly, we ought to track the current alignment of the allocation pointer
 * w.r.t. the current fragment.
 */

        // write the object descriptor
        cxt->createStoreInt (cxt->uConst(this->_v_desc.toUInt64()),allocPtr);

        // compute the object's address and cast it to a ML value
        llvm::Value *obj = cxt->asMLValue (cxt->createGEP (allocPtr, 1));

        // initialize the object's fields
        int offset = 0;
        for (int i = 0;  i < len;  ++i) {
            auto fld = this->_v_fields[i];
            int szb = bitsToBytes(fld->get_sz());
            if ((offset & (szb - 1)) != 0) {
                // align the offset
                offset = (offset + (szb - 1)) & ~(szb - 1);
            }
            llvm::Type *elemTy = numType (cxt, fld->get_kind(), fld->get_sz());
            llvm::Type *argTy = args[i]->getType();
            // we are using byte offsets, so we need a byte pointer to the
            // field location (obj+offset)
            auto adr = cxt->createGEP (cxt->i8Ty, obj, offset);
            if (argTy == cxt->mlValueTy) {
                // the argument and field types should have the same size
                // Note that in some cases, the field type might be "f64",
                // but we treat it as raw pointer-sized data, since we are
                // just writing it into memory
                assert (
                    (elemTy->getPrimitiveSizeInBits() == cxt->intTy->getPrimitiveSizeInBits())
                    && "expected native integer field");
                cxt->createStoreML (args[i], adr);
            }
            else {
                assert (args[i]->getType() == elemTy && "type mismatch");
                cxt->createStore (args[i], adr, szb);
            }
            offset += szb;
        }
      // align the final offset to the native word size
        offset = (offset + (cxt->wordSzInBytes() - 1)) & ~(cxt->wordSzInBytes() - 1);

      // bump the allocation pointer
        cxt->setMLReg (smlnj::cfgcg::CMRegId::ALLOC_PTR, cxt->createGEP (obj, offset));

        return obj;

    } // RAW_RECORD::codegen

    llvm::Value *RAW_ALLOC::codegen (smlnj::cfgcg::Context *cxt, Args_t const &args)
    {
        llvm::Value *allocPtr = cxt->mlReg (smlnj::cfgcg::CMRegId::ALLOC_PTR);
        int len = this->_v_len;  // length in bytes
        int align = this->_v_align; // alignment in bytes

        if (this->_v_desc.has_value()) {
            if (align > cxt->wordSzInBytes()) {
              // adjust the allocation point to be one word before an aligned address
                allocPtr = cxt->createIntToPtr(
                    cxt->createOr(
                        cxt->createPtrToInt(allocPtr),
                        cxt->uConst(align - cxt->wordSzInBytes())));
            }
          // write object descriptor
            uint64_t desc = this->_v_desc.value().toUInt64();
            cxt->createStoreML (cxt->uConst(desc), allocPtr);
        }
        else {
            // else tagless object for spilling
            assert (align == cxt->wordSzInBytes()
                && "unexpected alignment for spill record");
        }

      // compute the object's address and cast it to an ML value
        llvm::Value *obj = cxt->createBitCast (
            cxt->createGEP (allocPtr, 1),
            cxt->mlValueTy);

      // bump the allocation pointer
        cxt->setMLReg (smlnj::cfgcg::CMRegId::ALLOC_PTR,
            cxt->createBitCast (
                cxt->createGEP (cxt->i8Ty, allocPtr, len + cxt->wordSzInBytes()),
                cxt->ptrTy));

        return obj;

    } // RAW_ALLOC::codegen


  /***** code generation for the `arith` type *****/

    llvm::Value *ARITH::codegen (smlnj::cfgcg::Context *cxt, Args_t const &argv)
    {
        llvm::Value *pair;

        assert ((argv.size() == 2) && "expected two arguments");

        unsigned sz = this->_v_sz;
        std::vector<llvm::Value *> args = {
            cxt->asInt(sz, argv[0]), cxt->asInt(sz, argv[1])
          };

        switch (this->get_oper()) {
            case arithop::IADD:
                pair = cxt->build().CreateCall(
                    (this->get_sz() == 32) ? cxt->sadd32WOvflw() : cxt->sadd64WOvflw(),
                    args);
                break;

            case arithop::ISUB:
                pair = cxt->build().CreateCall(
                    (this->get_sz() == 32) ? cxt->ssub32WOvflw() : cxt->ssub64WOvflw(),
                    args);
                break;

            case arithop::IMUL:
                pair = cxt->build().CreateCall(
                    (this->get_sz() == 32) ? cxt->smul32WOvflw() : cxt->smul64WOvflw(),
                    args);
                break;

            case arithop::IDIV:
              // can trap on `minInt / ~1`, but the x86-64 hardware generates that trap,
              // so we do not need to do anything special.  May want to add explicit
              // test in the future.
                return cxt->createSDiv (args[0], args[1]);

            case arithop::IREM:
                return cxt->createSRem (args[0], args[1]);
        }

        llvm::Value *res = cxt->createExtractValue(pair, 0);
        llvm::Value *obit = cxt->createExtractValue(pair, 1);
        llvm::BasicBlock *next = cxt->newBB ("ok");
        cxt->build().CreateCondBr(obit, cxt->getOverflowBB(), next, cxt->overflowWeights());
      // switch to the new block for the continuation
        cxt->setInsertPoint (next);

        return res;

    } // ARITH::codegen

    llvm::Value *FLOAT_TO_INT::codegen (smlnj::cfgcg::Context *cxt, Args_t const &args)
    {
        return cxt->createFPToSI (args[0], cxt->iType (this->_v_to));

    } // FLOAT_TO_INT::codegen


  /***** code generation for the `pure` type *****/

    llvm::Value *PURE_ARITH::codegen (smlnj::cfgcg::Context *cxt, Args_t const &args)
    {
        unsigned sz = this->_v_sz;
        switch (this->get_oper()) {
            case pureop::ADD:
                return cxt->createAdd(cxt->asInt(sz, args[0]), cxt->asInt(sz, args[1]));
            case pureop::SUB:
                return cxt->createSub(cxt->asInt(sz, args[0]), cxt->asInt(sz, args[1]));
            case pureop::MUL:
                return cxt->createMul(cxt->asInt(sz, args[0]), cxt->asInt(sz, args[1]));
            case pureop::SDIV:
                return cxt->createSDiv(cxt->asInt(sz, args[0]), cxt->asInt(sz, args[1]));
            case pureop::SREM:
                return cxt->createSRem(cxt->asInt(sz, args[0]), cxt->asInt(sz, args[1]));
            case pureop::UDIV:
                return cxt->createUDiv(cxt->asInt(sz, args[0]), cxt->asInt(sz, args[1]));
            case pureop::UREM:
                return cxt->createURem(cxt->asInt(sz, args[0]), cxt->asInt(sz, args[1]));
            case pureop::SHL:
                return cxt->createShl(cxt->asInt(sz, args[0]), cxt->asInt(sz, args[1]));
            case pureop::ASHR:
                return cxt->createAShr(cxt->asInt(sz, args[0]), cxt->asInt(sz, args[1]));
            case pureop::LSHR:
                return cxt->createLShr(cxt->asInt(sz, args[0]), cxt->asInt(sz, args[1]));
            case pureop::ORB:
                return cxt->createOr(cxt->asInt(sz, args[0]), cxt->asInt(sz, args[1]));
            case pureop::XORB:
                return cxt->createXor(cxt->asInt(sz, args[0]), cxt->asInt(sz, args[1]));
            case pureop::ANDB:
                return cxt->createAnd(cxt->asInt(sz, args[0]), cxt->asInt(sz, args[1]));
            case pureop::CNTPOP:
                return cxt->build().CreateCall(
                    (this->get_sz() == 32) ? cxt->ctpop32() : cxt->ctpop64(),
                    args);
            case pureop::CNTLZ: {
                // we add `false` as an argument to get a result for zero
                llvm::Value *xargs[2] = { args[0], cxt->build().getFalse() };
                return cxt->build().CreateCall(
                    (this->get_sz() == 32) ? cxt->ctlz32() : cxt->ctlz64(),
                    xargs);
            }
            case pureop::CNTTZ: {
                // we add `false` as an argument to get a result for zero
                llvm::Value *xargs[2] = { args[0], cxt->build().getFalse() };
                return cxt->build().CreateCall(
                    (this->get_sz() == 32) ? cxt->cttz32() : cxt->cttz64(),
                    xargs);
            }
            case pureop::ROTL:
            case pureop::ROTR:
                assert (false && "rotations unimplemented");
            case pureop::FADD:
                return cxt->createFAdd(args[0], args[1]);
            case pureop::FSUB:
                return cxt->createFSub(args[0], args[1]);
            case pureop::FMUL:
                return cxt->createFMul(args[0], args[1]);
            case pureop::FDIV:
                return cxt->createFDiv(args[0], args[1]);
            case pureop::FREM:
                return cxt->createFRem(args[0], args[1]);
            case pureop::FMADD:
                 return cxt->build().CreateCall(
                    (this->get_sz() == 32) ? cxt->fma32() : cxt->fma64(),
                    args);
            case pureop::FNEG:
                return cxt->createFNeg(args[0]);
            case pureop::FABS:
                 return cxt->build().CreateCall(
                    (this->get_sz() == 32) ? cxt->fabs32() : cxt->fabs64(),
                    args);
            case pureop::FCOPYSIGN:
                 return cxt->build().CreateCall(
                    (this->get_sz() == 32) ? cxt->copysign32() : cxt->copysign64(),
                    args);
            case pureop::FSQRT:
                 return cxt->build().CreateCall(
                    (this->get_sz() == 32) ? cxt->sqrt32() : cxt->sqrt64(),
                    args);
        } // switch

    } // PURE_ARITH::codegen

    llvm::Value *EXTEND::codegen (smlnj::cfgcg::Context *cxt, Args_t const &args)
    {
        // the current handling of smaller integer types in the SML/NJ backend is
        // kind of broken, since small-integers are usually represented as tagged
        // values.  This means that if the argument is an MLvalue this operation
        // is a no-op
        if (args[0]->getType() != cxt->mlValueTy) {
            if (this->_v_signed) {
                return cxt->createSExt (args[0], cxt->iType(this->_v_to));
            } else {
                return cxt->createZExt (args[0], cxt->iType(this->_v_to));
            }
        }
        else {
            assert (! this->_v_signed && "unexpected sign extension of ML llvm::Value");
            assert (cxt->iType(this->_v_to) == cxt->intTy);
            return args[0];
        }

    } // EXTEND::codegen

    llvm::Value *TRUNC::codegen (smlnj::cfgcg::Context *cxt, Args_t const &args)
    {
        return cxt->createTrunc (args[0], cxt->iType(this->_v_to));

    } // TRUNC::codegen

    llvm::Value *INT_TO_FLOAT::codegen (smlnj::cfgcg::Context *cxt, Args_t const &args)
    {
        return cxt->createSIToFP (args[0], cxt->fType(this->_v_to));

    } // INT_TO_FLOAT::codegen

    llvm::Value *FLOAT_TO_BITS::codegen (smlnj::cfgcg::Context *cxt, Args_t const &args)
    {
        return cxt->createBitCast (args[0], cxt->iType(this->_v_sz));

    } // FLOAT_TO_BITS::codegen

    llvm::Value *BITS_TO_FLOAT::codegen (smlnj::cfgcg::Context *cxt, Args_t const &args)
    {
        return cxt->createBitCast (args[0], cxt->fType(this->_v_sz));

    } // BITS_TO_FLOAT::codegen

    llvm::Value *PURE_SUBSCRIPT::codegen (smlnj::cfgcg::Context *cxt, Args_t const &args)
    {
        llvm::Value *adr = cxt->createGEP (cxt->asPtr(args[0]), cxt->asInt(args[1]));
        return cxt->build().CreateLoad (cxt->mlValueTy, adr);

    } // PURE_SUBSCRIPT::codegen

    llvm::Value *PURE_RAW_SUBSCRIPT::codegen (smlnj::cfgcg::Context *cxt, Args_t const &args)
    {
        llvm::Type *elemTy = numType (cxt, this->_v_kind, this->_v_sz);

        llvm::Value *adr = cxt->createGEP (elemTy, args[0], args[1]);

        return cxt->createLoad (elemTy, adr, bitsToBytes(this->_v_sz));

    } // PURE_RAW_SUBSCRIPT::codegen

    llvm::Value *RAW_SELECT::codegen (smlnj::cfgcg::Context *cxt, Args_t const &args)
    {
        llvm::Type *elemTy = numType (cxt, this->_v_kind, this->_v_sz);

/* QUESTION: should we specialize the case where the offset is 0? */
        llvm::Value *adr = cxt->createGEP (
                cxt->i8Ty,
                cxt->asPtr(args[0]),
                cxt->uConst (this->_v_offset));

        return cxt->createLoad (elemTy, adr, bitsToBytes(this->_v_sz));

    } // RAW_SELECT::codegen


  /***** code generation for the `looker` type *****/

    llvm::Value *DEREF::codegen (smlnj::cfgcg::Context *cxt, Args_t const &args)
    {
        return cxt->build().CreateLoad (cxt->mlValueTy, cxt->asPtr(args[0]));

    } // DEREF::codegen

    llvm::Value *SUBSCRIPT::codegen (smlnj::cfgcg::Context *cxt, Args_t const &args)
    {
        llvm::Value *baseAdr = cxt->asPtr(args[0]);
        llvm::Value *adr = cxt->createGEP(baseAdr, cxt->asInt(args[1]));
// QUESTION: should we mark the load as volatile?
        return cxt->build().CreateLoad (cxt->mlValueTy, adr);

    } // SUBSCRIPT::codegen

    llvm::Value *RAW_LOAD::codegen (smlnj::cfgcg::Context *cxt, Args_t const &args)
    {
        llvm::Type *elemTy = numType (cxt, this->_v_kind, this->_v_sz);

      // RAW_LOAD assumes byte addressing, so we compute the address as a `char *`
        llvm::Value *adr = cxt->createGEP (cxt->i8Ty, args[0], args[1]);

// QUESTION: should we mark the load as volatile?
        return cxt->createLoad (elemTy, adr, bitsToBytes(this->_v_sz));

    } // RAW_LOAD::codegen

    llvm::Value *RAW_SUBSCRIPT::codegen (smlnj::cfgcg::Context *cxt, Args_t const &args)
    {
        llvm::Type *elemTy = numType (cxt, this->_v_kind, this->_v_sz);

        llvm::Value *adr = cxt->createGEP (elemTy, args[0], args[1]);

// QUESTION: should we mark the load as volatile?
        return cxt->createLoad (elemTy, adr, bitsToBytes(this->_v_sz));

    } // RAW_SUBSCRIPT::codegen

    llvm::Value *GET_HDLR::codegen (smlnj::cfgcg::Context *cxt, Args_t const &args)
    {
        return cxt->mlReg (smlnj::cfgcg::CMRegId::EXN_HNDLR);

    } // GET_HDLR::codegen

    llvm::Value *GET_VAR::codegen (smlnj::cfgcg::Context *cxt, Args_t const &args)
    {
        return cxt->mlReg (smlnj::cfgcg::CMRegId::VAR_PTR);

    } // GET_VAR::codegen


  /***** code generation for the `setter` type *****/

  // utility function to generate a store-list update
    inline void recordStore (smlnj::cfgcg::Context *cxt, llvm::Value *adr)
    {
        llvm::Value *allocPtr = cxt->mlReg (smlnj::cfgcg::CMRegId::ALLOC_PTR);
        llvm::Value *storePtr = cxt->mlReg (smlnj::cfgcg::CMRegId::STORE_PTR);

      // write the address into the store-list object
        cxt->createStoreML (adr, cxt->createGEP (allocPtr, 0));
      // write the link field
        cxt->createStoreML (storePtr, cxt->createGEP (allocPtr, 1));
      // update the store pointer
        cxt->setMLReg (smlnj::cfgcg::CMRegId::STORE_PTR, allocPtr);
      // bump the allocation pointer
        cxt->setMLReg (smlnj::cfgcg::CMRegId::ALLOC_PTR, cxt->createGEP (allocPtr, 2));

    }

    void UNBOXED_UPDATE::codegen (smlnj::cfgcg::Context *cxt, Args_t const &args)
    {
        cxt->createStoreML (args[2], cxt->createGEP (cxt->mlValueTy, args[0], args[1]));

    } // UNBOXED_UPDATE::codegen

    void UPDATE::codegen (smlnj::cfgcg::Context *cxt, Args_t const &args)
    {
        llvm::Value *adr = cxt->createGEP (cxt->mlValueTy, args[0], args[1]);

        recordStore (cxt, adr);

        cxt->createStoreML (args[2], adr);

    } // UPDATE::codegen

    void UNBOXED_ASSIGN::codegen (smlnj::cfgcg::Context *cxt, Args_t const &args)
    {
        cxt->createStore (
            cxt->asInt(args[1]),
            cxt->asMLValue (args[0]),
            cxt->wordSzInBytes());

    } // UNBOXED_ASSIGN::codegen

    void ASSIGN::codegen (smlnj::cfgcg::Context *cxt, Args_t const &args)
    {
        recordStore (cxt, args[0]);

        cxt->createStoreML (args[1], args[0]);

    } // ASSIGN::codegen

    void RAW_UPDATE::codegen (smlnj::cfgcg::Context *cxt, Args_t const &args)
    {
        llvm::Type *elemTy = numType (cxt, this->_v_kind, this->_v_sz);

        llvm::Value *adr = cxt->createGEP (elemTy, args[0], args[1]);

        if (args[2]->getType() == cxt->mlValueTy) {
            assert (elemTy == cxt->intTy && "expected native integer field");
            return cxt->createStoreML (args[2], adr);
        }
        else {
            return cxt->createStore (args[2], adr, bitsToBytes(this->_v_sz));
        }

    } // RAW_UPDATE::codegen

    void RAW_STORE::codegen (smlnj::cfgcg::Context *cxt, Args_t const &args)
    {
        llvm::Type *elemTy = numType (cxt, this->_v_kind, this->_v_sz);

      // RAW_STORE assumes byte addressing, so we compute the address as a `char *`
      // and then bitcast to the desired pointer type for the store
        llvm::Value *adr = cxt->createGEP (cxt->i8Ty, args[0], args[1]);

        return cxt->createStore (args[2], adr, bitsToBytes(this->_v_sz));

    } // RAW_STORE::codegen

    void SET_HDLR::codegen (smlnj::cfgcg::Context *cxt, Args_t const &args)
    {
        cxt->setMLReg (smlnj::cfgcg::CMRegId::EXN_HNDLR, args[0]);

    } // SETHDLR::codegen

    void SET_VAR::codegen (smlnj::cfgcg::Context *cxt, Args_t const &args)
    {
        cxt->setMLReg (smlnj::cfgcg::CMRegId::VAR_PTR, args[0]);

    } // SETVAR::codegen


  /***** code generation for the `branch` type *****/

    static llvm::CmpInst::Predicate ICmpMap[] = {
            llvm::ICmpInst::ICMP_SGT,   // signed GT
            llvm::ICmpInst::ICMP_UGT,   // unsigned GT
            llvm::ICmpInst::ICMP_SGE,   // signed GTE
            llvm::ICmpInst::ICMP_UGE,   // unsigned GTE
            llvm::ICmpInst::ICMP_SLT,   // signed LT
            llvm::ICmpInst::ICMP_ULT,   // unsigned LT
            llvm::ICmpInst::ICMP_SLE,   // signed LTE
            llvm::ICmpInst::ICMP_ULE,   // unsigned LTE
            llvm::ICmpInst::ICMP_EQ,    // (signed) EQL
            llvm::ICmpInst::ICMP_EQ,    // (unsigned) EQL
            llvm::ICmpInst::ICMP_NE,    // (signed) NEQ
            llvm::ICmpInst::ICMP_NE     // (unsigned) NEQ
        };

    llvm::Value *CMP::codegen (smlnj::cfgcg::Context *cxt, Args_t const &args)
    {
        int idx = 2 * (static_cast<int>(this->_v_oper) - 1);
        if (! this->_v_signed) {
            idx += 1;
        }

        return cxt->createICmp (
            ICmpMap[idx],
            cxt->asInt(this->_v_sz, args[0]),
            cxt->asInt(this->_v_sz, args[1]));

    } // CMP::codegen

    static llvm::CmpInst::Predicate FCmpMap[] = {
            llvm::FCmpInst::FCMP_OEQ,   // F_EQ
            llvm::FCmpInst::FCMP_UNE,   // F_ULG
            llvm::FCmpInst::FCMP_ONE,   // F_UN
            llvm::FCmpInst::FCMP_ORD,   // F_LEG
            llvm::FCmpInst::FCMP_OGT,   // F_GT
            llvm::FCmpInst::FCMP_OGE,   // F_GE
            llvm::FCmpInst::FCMP_UGT,   // F_UGT
            llvm::FCmpInst::FCMP_UGE,   // F_UGE
            llvm::FCmpInst::FCMP_OLT,   // F_LT
            llvm::FCmpInst::FCMP_OLE,   // F_LE
            llvm::FCmpInst::FCMP_ULT,   // F_ULT
            llvm::FCmpInst::FCMP_ULE,   // F_ULE
            llvm::FCmpInst::FCMP_ONE,   // F_LG
            llvm::FCmpInst::FCMP_UEQ    // F_UE
        };

    llvm::Value *FCMP::codegen (smlnj::cfgcg::Context *cxt, Args_t const &args)
    {
        return cxt->createFCmp (FCmpMap[static_cast<int>(this->_v_oper) - 1], args[0], args[1]);

    } // FCMP::codegen

    llvm::Value *FSGN::codegen (smlnj::cfgcg::Context *cxt, Args_t const &args)
    {
      // bitcast to integer type of same size
        llvm::Value *asInt = cxt->createBitCast (args[0], cxt->iType (this->_v0));

        return cxt->createICmpSLT(asInt, cxt->iConst(this->_v0, 0));

    } // FSGN::codegen

    llvm::Value *PEQL::codegen (smlnj::cfgcg::Context *cxt, Args_t const &args)
    {
        return cxt->createICmpEQ(args[0], args[1]);

    } // PEQL::codegen

    llvm::Value *PNEQ::codegen (smlnj::cfgcg::Context *cxt, Args_t const &args)
    {
        return cxt->createICmpNE(args[0], args[1]);

    } // PNEQ::codegen

    llvm::Value *LIMIT::codegen (smlnj::cfgcg::Context *cxt, Args_t const &args)
    {
        unsigned int amt = this->_v0;
        unsigned int allocSlop = cxt->targetInfo()->allocSlopSzb;
        if (amt > allocSlop) {
            return cxt->createICmp(llvm::ICmpInst::ICMP_UGT,
                cxt->createAdd (
                    cxt->mlReg(smlnj::cfgcg::CMRegId::ALLOC_PTR),
                    cxt->uConst(amt - allocSlop)),
                cxt->asInt (cxt->mlReg(smlnj::cfgcg::CMRegId::LIMIT_PTR)));
        } else {
            return cxt->createICmp(llvm::ICmpInst::ICMP_UGT,
                cxt->asInt (cxt->mlReg(smlnj::cfgcg::CMRegId::ALLOC_PTR)),
                cxt->asInt (cxt->mlReg(smlnj::cfgcg::CMRegId::LIMIT_PTR)));
        }

    } // LIMIT::codegen

} // namespace CFG_Prim
