(* ex9.sml
 *
 * COPYRIGHT (c) 2021 The Fellowship of SML/NJ (http://www.smlnj.org)
 * All rights reserved.
 *
 * This code tests how floating-point negation and absolute-value
 * are handled.
 *
 * Hand-crafted CFG for the function
 *
 *      fun f x = ~(Real.abs x);
 *
 * The generated first-order CPS is:
 *
 * ***********************************************
 * std v68(v83[PV],v82[PV],v81[C],v80[PV],v79[PV],v78[PV],v77[R64]) =
 *    fabsf64(v77) -> v47[R64]
 *    negf64(v47) -> v46[R64]
 *    v81(v81,v80,v79,v78,v46)
 * ***********************************************
 *
 * The equivalent CFG IR (with types and GC code omitted) is
 *
 * ***********************************************
 *   std_fun (L)v68 (v83:ptr, v82:ptr, v81:label, v80:ptr, v79:ptr, v78:ptr, v77:f64) {
 *     throw v81 (v81:label, v80:ptr, v79:ptr, v78:ptr, fneg64(fabs64(v77)):f64)
 *   }
 * ***********************************************
 *)

structure Ex9 =
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

      val fn68 = C.Cluster{
	      attrs = attrs true,
	      frags = [
	        C.Frag{
		  kind = C.STD_FUN,
		  lab = v 68,
		  params = mkParams [
		      (v 83, C.PTRt), (v 82, C.PTRt), (v 81, C.LABt),
		      (v 80, C.PTRt), (v 79, C.PTRt), (v 78, C.PTRt),
		      (v 77, C.FLTt{sz=64})
		    ],
		  body = C.THROW (V 81, [
			V 81, V 80, V 79, V 78,
			pure (P.FNEG, [pure(P.FABS, [V 77])])
		      ],
		    [C.LABt, C.PTRt, C.PTRt, C.PTRt, C.FLTt{sz=64}])
		}]
	    }
    in
    val cu = {srcFile = "ex9.sml", entry = fn68, fns = []}
    end (* local *)

  end
