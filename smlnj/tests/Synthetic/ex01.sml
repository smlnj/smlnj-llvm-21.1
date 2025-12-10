(* ex01.sml
 *
 * COPYRIGHT (c) 2025 The Fellowship of SML/NJ (https://smlnj.org)
 * All rights reserved.
 *
 * Hand-crafted CFG for the function
 *
 *	fun f x = x;
 *
 * The generated first-order CPS is:
 *
 * ***********************************************
 *	std v78(v79[PV],v38[PV],v60[C],v61[PV],v62[PV],v63[PV],v45[PR2]) =
 *	   {RK_ESCAPE 1,(L)v64} -> v95
 *	   {v95} -> v96
 *	   v60(v60,v61,v62,v63,v96)
 *
 *	std v64(v86[PV],v85[PV],v84[C],v83[PV],v82[PV],v81[PV],v80[PR1]) =
 *	   {RK_ESCAPE 1,(L)v71} -> v94
 *	   v84(v84,v83,v82,v81,v94)
 *
 *	std v71(v93[PV],v92[PV],v91[C],v90[PV],v89[PV],v88[PV],v87[PV]) =
 *	   v91(v91,v90,v89,v88,v87)
 * ***********************************************
 *)

structure Ex1 =
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
      fun gcTest (n, stm1, stm2) =
	    C.BRANCH(P.LIMIT(Word.fromInt n), [], 1, stm1, stm2)
      fun attrs bp = { (* cluster attrs *)
	      alignHP = 8, needsBasePtr = bp, hasTrapArith = false, hasRCC = false
	    }
      val unkProb = 0

      val fn78 = C.Cluster{
	      attrs = attrs true,
	      frags = [
	        C.Frag{
		  kind = C.STD_FUN,
		  lab = v 78,
		  params = mkParams [
		      (v 79, C.PTRt), (v 38, C.PTRt), (v 60, C.LABt), (v 61, C.PTRt),
		      (v 62, C.PTRt), (v 63, C.PTRt), (v 45, C.PTRt)
		    ],
		  body = gcTest (0,
		    (* THEN *)
		      C.APPLY (LAB 100,
			[V 79, V 38, V 60, V 61, V 62, V 63, V 45],
			[C.PTRt, C.PTRt, C.PTRt, C.PTRt, C.PTRt, C.PTRt, C.PTRt]),
		    (* ELSE *)
		      record ([LAB 64], v 95,
		      record ([V 95], v 96,
			C.THROW (V 60,
			  [V 60, V 61, V 62, V 63, V 96],
			  [C.LABt, C.PTRt, C.PTRt, C.PTRt, C.PTRt]))))
		}]
	    }
      val fn64 = C.Cluster{
	      attrs = attrs true,
	      frags = [
	        C.Frag{
		  kind = C.STD_FUN,
		  lab = v 64,
		  params = mkParams [
		      (v 86, C.PTRt), (v 85, C.PTRt), (v 84, C.LABt),
		      (v 83, C.PTRt), (v 82, C.PTRt), (v 81, C.PTRt), (v 80, C.PTRt)
		    ],
		  body = gcTest (0,
		    (* THEN *)
		      C.APPLY (LAB 100,
			[V 86, V 85, V 84, V 83, V 82, V 81, V 80],
			[C.PTRt, C.PTRt, C.PTRt, C.PTRt, C.PTRt, C.PTRt, C.PTRt]),
		    (* ELSE *)
		      record ([LAB 71], v 94,
			C.THROW (V 84,
			  [V 84, V 83, V 82, V 81, V 94],
			  [C.PTRt, C.PTRt, C.PTRt, C.PTRt, C.PTRt])))
		}]
	    }
      val fn71 = C.Cluster{
	      attrs = attrs false,
	      frags = [
	        C.Frag{
		  kind = C.STD_FUN,
		  lab = v 71,
		  params = mkParams [
		      (v 93, C.PTRt), (v 92, C.PTRt), (v 91, C.LABt), (v 90, C.PTRt),
		      (v 89, C.PTRt), (v 88, C.PTRt), (v 87, C.PTRt)
		    ],
		  body = gcTest (0,
		    (* THEN *)
		      C.APPLY (LAB 100,
			[V 93, V 92, V 91, V 90, V 89, V 88, V 87],
			[C.PTRt, C.PTRt, C.PTRt, C.PTRt, C.PTRt, C.PTRt, C.PTRt]),
		    (* ELSE *)
		      C.THROW(V 91,
			[V 91, V 90, V 89, V 88, V 87],
			[C.PTRt, C.PTRt, C.PTRt, C.PTRt, C.PTRt]))
		}]
	    }
    (* the shared GC fragment *)
      val fn100 = C.Cluster{
	      attrs = attrs false,
	      frags = [
	        C.Frag{
		  kind = C.STD_FUN,
		  lab = v 100,
		  params = mkParams [
		      (v 101, C.PTRt), (v 102, C.PTRt), (v 103, C.LABt), (v 104, C.PTRt),
		      (v 105, C.PTRt), (v 106, C.PTRt), (v 107, C.PTRt)
		    ],
		  body = C.CALLGC(
		    [V 101, V 102, V 103, V 104, V 105, V 106, V 107],
		    [v 111, v 112, v 113, v 114, v 115, v 116, v 117],
		      C.APPLY(V 111,
			[V 111, V 112, V 113, V 114, V 115, V 116, V 117],
			[C.PTRt, C.PTRt, C.PTRt, C.PTRt, C.PTRt, C.PTRt, C.PTRt]))
		}]
	    }
    in
    val cu = {srcFile = "id.sml", entry = fn78, fns = [fn64, fn71, fn100]}
    end (* local *)

  end
