(* ex07.sml
 *
 * COPYRIGHT (c) 2025 The Fellowship of SML/NJ (https://smlnj.org)
 * All rights reserved.
 *
 * Hand-crafted CFG for the function
 *
 *      datatype t = Lf of word | Nd of t * t;
 *
 *      fun sum (Lf w) = w | sum (Nd(t1, t2)) = sum t1 + sum t2;
 *
 * The generated first-order CPS is:
 *
 * ***********************************************
 *      std v127(v128[PV],v62[PV],v106[C],v107[PV],v108[PV],v109[PV],v72[PR2]) =
 *         {RK_ESCAPE 1,(L)v110} -> v160
 *         {v160} -> v161
 *         v106(v106,v107,v108,v109,v161)
 ***
 *      std v110(v135[PV],v134[PV],v133[C],v132[PV],v131[PV],v130[PV],v129[PV]) =
 *         (L)v117(v133,v132,v131,v130,v129)
 *
 *      known_chk v117(v140[C],v139[PV],v138[PV],v137[PV],v136[PV]) =
 *         v136.0 -> v141[I]
 *         if u63<>(v141,(I63t)0) [v100] then
 *            v136.1 -> v142[PR2]
 *            v142.0 -> v143[PV]
 *            v142.1 -> v144[PV]
 *            {RK_CONT 3,v140,v139,v138} -> v158
 *            (L)v117((L)v122,v158,v144,v137,v143)
 *         else
 *            v136.1 -> v159[I]
 *            v140(v140,v139,v138,v137,v159)
 *
 *      std_cont v122(v149[PV],v148[PV],v147[PV],v146[PV],v145[I]) =
 *         (L)v117((L)v125,v148,v145,v146,v147)
 ***
 *      std_cont v125(v154[PV],v153[PV],v152[I],v151[PV],v150[I]) =
 *         addu63(v152,v150) -> v63[I]
 *         v153.2 -> v155[PV]
 *         v153.1 -> v156[PV]
 *         v153.0 -> v157[C]
 *         v157(v157,v156,v155,v151,v63)
 * ***********************************************
 *
 * Notice that the second cluster here has two entries (v110 and v125), which
 * requires some restructuring to satisfy the single-entry requirement of LLVM.
 * The resulting CFG IR (with types omitted) is below, where we have added an
 * "x" suffix to fresh lambda variables that we have to introduce.
 *
 * ***********************************************
 *      FUN v127 (v128, v62, v106, v107, v108, v109, v72) =
 *         ALLOC(RECORD 0x80, [LABEL v110]) -> v160
 *         ALLOC(RECORD 0x80, [VAR v160]) -> v161
 *            THROW (VAR v106) (VAR v106, VAR v107, VAR v108, VAR v109, VAR v161)
 ***
 *	FUN v110 (v135, v134, v133, v132, v131, v130, v129) =
 *         APPLY (LABEL v117x) (VAR v133, VAR v132, VAR v131, VAR v130, VAR v129)
 ***
 *	FUN v117x (v140x, v139x, v138x, v137x, v136x) =
 *	   GOTO v117 (VAR v140x, VAR v139x, VAR v138x, VAR v137x, VAR v136x)
 *
 *	FRAG v117 (v140, v139, v138, v137, v136) =
 *	   IF (NEQ(SELECT(0, VAR v136), 1) THEN
 *            LET SELECT(1, VAR v136) -> v142
 *		 ALLOC(RECORD 0x180, [VAR v140, VAR v139, VAR v138]) -> v158
 *               GOTO v117 (LABEL v122, VAR v158, VAR v144, VAR v137, VAR v143)
 *         ELSE
 *            THROW (VAR v140)
 *              (VAR v140, VAR v139, VAR v138, VAR v137, SELECT(1, VAR v136))
 ***
 *      CONT v122 (v149, v148, v147, v146, v145) =
 *         APPLY (LABEL v117x) (LABEL v125, VAR v148, VAR v145, VAR v146, VAR v147)
 ***
 *      CONT v125 (v154, v153, v152, v151, v150) =
 *	   LET SELECT(0, VAR v153) -> v157
 *	      THROW (VAR v157)
 *		 (VAR v157, SELECT(1, VAR v153), SELECT(2, VAR v153), VAR v151,
 *	            ADD(VAR v152, VAR v150))
 * ***********************************************
 *)

structure Ex7 =
  struct

    local
      structure P = CFG_Prim
      structure C = CFG
      structure II = IntInf
      fun v id = LambdaVar.fromId id
      fun V id = C.VAR(v id)
      fun LAB id = C.LABEL(v id)
      fun mkParam (x : LambdaVar.lvar, ty : C.ty) = {name = x, ty = ty}
      val mkParams = List.map mkParam
      fun num n = C.NUM{iv=n, sz=64}
      fun record (flds, x, k) = let
	    val desc = ObjDesc.record(length flds)
	    in
	      C.ALLOC(P.RECORD{desc = desc, mut = false}, flds, x, k)
	    end
      fun pureOp (oper, args) = C.PURE(P.PURE_ARITH{oper=oper, sz=64}, args)
      fun attrs bp = { (* cluster attrs *)
	      alignHP = 8, needsBasePtr = bp, hasTrapArith = false, hasRCC = false
	    }
      val unkProb = 0

      val fn127 = C.Cluster{
	      attrs = attrs true,
	      frags = [
	        C.Frag{
		  kind = C.STD_FUN,
		  lab = v 127,
		  params = mkParams [
		      (v 128, C.PTRt), (v 62, C.PTRt), (v 106, C.LABt),
		      (v 107, C.PTRt), (v 108, C.PTRt), (v 109, C.PTRt),
		      (v 72, C.PTRt)
		    ],
		  body = record ([LAB 110], v 160,
		    record ([V 160], v 161,
		      C.THROW (V 106,
			[V 106, V 107, V 108, V 109, V 161],
			[C.LABt, C.PTRt, C.PTRt, C.PTRt, C.PTRt])))
		}]
	    }
      val fn110 = C.Cluster{
	      attrs = attrs false,
	      frags = [
	        C.Frag{
		  kind = C.STD_FUN,
		  lab = v 110,
		  params = mkParams [
		      (v 135, C.PTRt), (v 134, C.PTRt), (v 133, C.LABt),
		      (v 132, C.PTRt), (v 131, C.PTRt), (v 130, C.PTRt),
		      (v 129, C.PTRt)
		    ],
		  body =
		}]
	    }
      val fr117 = C.Frag{
	      kind = C.INTERNAL,
	      lab = v 117,
	      params = mkParams [
		],
	      body =
	    }
      val fn1170 = C.Cluster{
	      attrs = attrs true,
	      frags = [
	        C.Frag{
		  kind = C.KNOWN_FUN,
		  lab = v 1170,
		  params = mkParams [
		    ],
		  body =
		},
	      frags = [fn117]
	    }
      val fn122 = C.Cluster{
	      attrs = attrs true,
	      frags = [
	        C.Frag{
		  kind = C.STD_CONT,
		  lab = v 122,
		  params = mkParams [
		     (v 149, C.PTRt), (v 148, C.PTRt), (v 147, C.PTRt),
		     (v 146, C.PTRt), (v 145, C.TAGt)
		    ],
(* NOTE: we add an extra argument (LAB 1170) so that the callee can compute the base ptr *)
		  body = C.APPLY (LAB 1170,
		    [LAB 1170, LAB 125, V 148, V 145, V 146, V 147],
		    [C.LABt, C.LABt, C.PTRt, C.NUMt{sz=64}, C.PTRt, C.PTRt])
		}]
	    }
      val fn125 = C.Cluster{
	      attrs = attrs false,
	      frags = [
	        C.Frag{
		  kind = C.STD_CONT,
		  lab = v 125,
		  params = mkParams [
		      (v 154, C.PTRt), (v 153, C.PTRt), (v 152, C.TAGt),
		      (v 151, C.PTRt), (v 150, C.TAGt)
		    ],
		  body =
		}]
	    }
    in
    val cu = {srcFile = "tree-add.sml", entry = fn127, fns = [fn110, fn1170, fn122, fn125]}
    end (* local *)

  end
