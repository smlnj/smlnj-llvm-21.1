(* ex08.sml
 *
 * COPYRIGHT (c) 2025 The Fellowship of SML/NJ (https://smlnj.org)
 * All rights reserved.
 *
 * An example for the paper.
 *
 * Hand-crafted CFG for the function
 *
 *      fun f (n : Int64.int) = n + 42
 *
 * The generated first-order CPS is:
 *
 * ***********************************************
 *      std v214(v215[PV],v185[PV],v203[C],v204[PV],v205[PV],v206[PV],v191[PR2]) =
 *         {RK_ESCAPE 1,(L)v207} -> v223
 *         {v223} -> v224
 *         v203(v203,v204,v205,v206,v224)
 ***
 *      std v207(v222[PV],v221[PV],v220[C],v219[PV],v218[PV],v217[PV],v216[I64]) =
 *         iadd64(v216,(I64)42) -> v186[I64]
 *         v220(v220,v219,v218,v217,v186)
 * ***********************************************
 *
 * The equivalent CFG IR (with types omitted) is
 *
 * ***********************************************
 *      FUN v214 (v215[PV],v185[PV],v203[C],v204[PV],v205[PV],v206[PV],v191[PR2]) =
 *         ALLOC(RECORD 0x80, [LABEL v207]) -> v223
 *         ALLOC(RECORD 0x80, [VAR v223]) -> v224
 *           THROW (VAR v203) (VAR v203, VAR v204, VAR v205, VAR v206, VAR v224)
 *
 *      FUN v207 (v222[PV],v221[PV],v220[C],v219[PV],v218[PV],v217[PV],v216[I64]) =
 *         IADD64 (v216, NUM(42:i64)) -> v186
 *         THROW (VAR v220) (VAR v220, VAR v219, VAR v218, VAR v217, VAR v186)
 * ***********************************************
 *)

structure Ex8 =
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
      fun num n = C.NUM{iv=n, sz=64}
      fun record (flds, x, k) = let
	    val desc = ObjDesc.record(length flds)
	    in
	      C.ALLOC(P.RECORD{desc = desc, mut = false}, flds, x, k)
	    end
      fun arith (oper, args, res, k) =
	    C.ARITH(P.ARITH{oper=oper, sz=64}, args, mkParam(res, C.NUMt{sz=64}), k)
      fun attrs bp = { (* cluster attrs *)
	      alignHP = 8, needsBasePtr = bp, hasTrapArith = false, hasRCC = false
	    }
      val unkProb = 0

      val fn214 = C.Cluster{
	      attrs = attrs true,
	      frags = [
	        C.Frag{
		  kind = C.STD_FUN,
		  lab = v 214,
		  params = mkParams [
		      (v 215, C.PTRt), (v 185, C.PTRt), (v 203, C.LABt),
		      (v 204, C.PTRt), (v 205, C.PTRt), (v 206, C.PTRt),
		      (v 191, C.PTRt)
		    ],
		  body = record ([LAB 207], v 223,
		    record ([V 223], v 224,
		      C.THROW (V 203,
			[V 203, V 204, V 205, V 206, V 224],
			[C.LABt, C.PTRt, C.PTRt, C.PTRt, C.PTRt])))
		}]
	    }
      val fn207 = C.Cluster{
	      attrs = attrs true,
	      frags = [
	        C.Frag{
		  kind = C.STD_FUN,
		  lab = v 207,
		  params = mkParams [
		      (v 222, C.PTRt), (v 221, C.PTRt), (v 220, C.LABt),
		      (v 219, C.PTRt), (v 218, C.PTRt), (v 217, C.PTRt),
		      (v 216, C.NUMt{sz=64})
		    ],
		  body = arith(P.IADD, [V 216, num 42], v 186,
		      C.THROW(V 220,
			[V 220, V 219, V 218, V 217, V 186],
			[C.LABt, C.PTRt, C.PTRt, C.PTRt, C.NUMt{sz=64}]))
		}]
	    }
    in
    val cu = {srcFile = "ex8.sml", entry = fn214, fns = [fn207]}
    end (* local *)

  end
