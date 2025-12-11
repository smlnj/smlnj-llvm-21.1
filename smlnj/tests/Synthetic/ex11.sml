(* ex11.sml
 *
 * COPYRIGHT (c) 2025 The Fellowship of SML/NJ (https://smlnj.org)
 * All rights reserved.
 *
 * This code tests the operations for rotating words.
 *
 * Hand-crafted CFG for the function using the LLVM instruction
 *
 *      fun f1 x = Word64.rotateL x
 *      fun f2 x = Word64.rotateR x
 *
 * The generated first-order CPS for `f1` is:
 *
 * ***********************************************
 *   std v10(v11[PV],v12[PV],v13[C<>],v14[PV],v15[PV],v16[PV],v17[I64],v18[I]) =
 *      rotl64(v17,v18) -> v19[I64]
 *      v13(v13,v14,v15,v16,v19)
 * ***********************************************
 *
 * The equivalent CFG IR (with GC code omitted) is
 *
 * ***********************************************
 *   std_fun (L)v10 (v11:ptr, v12:ptr, v13:label, v14:ptr, v15:ptr, v16:ptr, v17:i64, v18:int) {
 *     throw v13 (v13:label, v14:ptr, v15:ptr, v16:ptr, rotl64(v110,lshr64(v109,(i64)1)):i64)
 *   }
 * ***********************************************
 *)

structure Ex11 =
  struct

    local
      structure P = CFG_Prim
      structure C = CFG
      structure II = IntInf
      fun v id = LambdaVar.fromId id
      fun V id = C.VAR{name = v id}
      fun LAB id = C.LABEL{name = v id}
      fun mkParam (x : LambdaVar.lvar, ty : C.ty) = {name = x, ty = ty}
      val mkParams = List.map mkParam
      fun pure (oper, args) = C.PURE{oper=P.PURE_ARITH{oper=oper, sz=64}, args=args}
      fun num64 n = C.NUM{iv=n, sz=64}
      fun attrs bp = { (* cluster attrs *)
	      alignHP = 8, needsBasePtr = bp, hasTrapArith = false, hasRCC = false
	    }

      fun mkFn (p, lab, a1, a2, a3, a4, a5, a6, a7, a8) = C.Cluster{
	      attrs = attrs false,
	      frags = [
	        C.Frag{
		  kind = C.STD_FUN,
		  lab = v lab,
		  params = mkParams [
		      (v a1, C.PTRt), (v a2, C.PTRt), (v a3, C.LABt),
		      (v a4, C.PTRt), (v a5, C.PTRt), (v a6, C.PTRt),
		      (v a7, C.NUMt{sz=64}), (v a8, C.TAGt)
		    ],
		  body = C.THROW (V a3, [
			V a3, V a4, V a5, V a6,
			pure (p, [V a7, pure (P.LSHR, [V a8, num64 1])])
		      ],
		    [C.LABt, C.PTRt, C.PTRt, C.PTRt, C.NUMt{sz=64}])
		}]
	    }

      val fn10 = mkFn (P.ROTL, 10, 11, 12, 13, 14, 15, 16, 17, 18)
      val fn20 = mkFn (P.ROTR, 20, 21, 22, 23, 24, 25, 26, 27, 28)

    in
    val cu = {srcFile = "ex11.sml", entry = fn10, fns = [fn20]}
    end (* local *)

  end
