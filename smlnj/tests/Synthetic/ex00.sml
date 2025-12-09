(* ex1.sml
 *
 * COPYRIGHT (c) 2020 The Fellowship of SML/NJ (http://www.smlnj.org)
 * All rights reserved.
 *
 * Stripped down example for identity function
 *
 *	fun f x = x;
 *
 * The generated first-order CPS is:
 *
 * ***********************************************
 *	std v71(v93[PV],v92[PV],v91[C],v90[PV],v89[PV],v88[PV],v87[PV]) =
 *	   v91(v91,v90,v89,v88,v87)
 * ***********************************************
 *
 * The equivalent CFG IR (with types omitted) is
 *
 * ***********************************************
 *      FUN fn71 (v93, v92, v91, v90, v89, v88, v87) =
 *	   IF (LIMIT(0)) THEN
 *	      GOTO v100 (VAR v93, VAR v92, VAR v91, VAR v90, VAR v89, VAR v88, VAR v87)
 *	   ELSE
 *            ALLOC(RECORD 0x80, [LABEL v155]) -> v156
 *            ALLOC(RECORD 0x80, [VAR v156]) -> v157
 *              THROW (VAR v111) (VAR v111, VAR v112, VAR v113, VAR v114, VAR v157)
 *
 *	FRAG v100 (v101, v102, v103, v104, v105, v106, v107) =
 *	   CALLGC (v101, v102, v103, v104, v105, v106, v107) -> (v111, v112, v113, v114, v115, v116, v117)
 *	   APPLY (VAR v111) (v111, v112, v113, v114, v115, v116, v117)
 * ***********************************************
 *)

structure Ex0 =
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
		  body = gcTest(0,
		    C.GOTO(v 100, [V 93, V 92, V 91, V 90, V 89, V 88, V 87]),
		    C.THROW(V 91,
		      [V 91, V 90, V 89, V 88, V 87],
		      [C.LABt, C.PTRt, C.PTRt, C.PTRt, C.PTRt]))
		},
		C.Frag{
		  kind = C.INTERNAL,
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
    val cu = {srcFile = "id.sml", entry = fn71, fns = []}
    end (* local *)

  end
