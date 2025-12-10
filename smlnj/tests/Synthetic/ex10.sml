(* ex10.sml
 *
 * COPYRIGHT (c) 2025 The Fellowship of SML/NJ (https://smlnj.org)
 * All rights reserved.
 *
 * This code tests how floating-point negation and absolute-value
 * are handled.
 *
 * Hand-crafted CFG for the function using the LLVM instruction
 *
 *      fun f1 x = Word64.popCount x
 *      fun f2 x = Word64.countLeadingZeros x
 *      fun f3 x = Word64.countTrailingZeros x
 *
 * The generated first-order CPS for `f1` is:
 *
 * ***********************************************
 * std v10 (v13[PV],v14[PV],v15[C],v16[PV],v17[PV],v18[PV],v19[I64]) =
 *    cntpop64(v19) -> v20[I64]
 *    v15(v15,v16,v17,v18,v20)
 * ***********************************************
 *
 * The equivalent CFG IR (with GC code omitted) is
 *
 * ***********************************************
 *   std_fun (L)v10 (v13:ptr, v14:ptr, v15:label, v16:ptr, v17:ptr, v18:ptr, v19:i64) {
 *     throw v15 (v15:label, v16:ptr, v17:ptr, v18:ptr, cntpop64(v19):i64)
 *     }
 *   }
 * ***********************************************
 *)

structure Ex10 =
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
      fun attrs bp = { (* cluster attrs *)
	      alignHP = 8, needsBasePtr = bp, hasTrapArith = false, hasRCC = false
	    }

      fun mkUnaryFn (p, lab, a1, a2, a3, a4, a5, a6, a7) = C.Cluster{
	      attrs = attrs false,
	      frags = [
	        C.Frag{
		  kind = C.STD_FUN,
		  lab = v lab,
		  params = mkParams [
		      (v a1, C.PTRt), (v a2, C.PTRt), (v a3, C.LABt),
		      (v a4, C.PTRt), (v a5, C.PTRt), (v a6, C.PTRt),
		      (v a7, C.NUMt{sz=64})
		    ],
		  body = C.THROW (V a3, [
			V a3, V a4, V a5, V a6,
			pure (p, [V a7])
		      ],
		    [C.LABt, C.PTRt, C.PTRt, C.PTRt, C.NUMt{sz=64}])
		}]
	    }

      val fn10 = mkUnaryFn (P.CNTPOP, 10, 13, 14, 15, 16, 17, 18, 19)
      val fn20 = mkUnaryFn (P.CNTLZ, 20, 23, 24, 25, 26, 27, 28, 29)
      val fn30 = mkUnaryFn (P.CNTTZ, 30, 33, 34, 35, 36, 37, 38, 39)

    in
    val cu = {srcFile = "ex10.sml", entry = fn10, fns = [fn20, fn30]}
    end (* local *)

  end
