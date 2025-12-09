(* ex2.sml
 *
 * COPYRIGHT (c) 2020 The Fellowship of SML/NJ (http://www.smlnj.org)
 * All rights reserved.
 *
 * Hand-crafted CFG for the function
 *
 *      fun rev xs = let
 *            fun rev' ([], xs') = xs'
 *              | rev' (x::xs, xs') = rev' (xs, x::xs')
 *            in
 *              rev' (xs, [])
 *            end;
 *
 * The generated first-order CPS is:
 *
 * ***********************************************
 *      std v130 (v131[PV], v31[PV], v111[C], v112[PV], v113[PV], v114[PV], v53[PR2]) =
 *         {RK_ESCAPE 1, (L)v115} -> v156
 *         {v156} -> v157
 *         v111 (v111, v112, v113, v114, v157)
 *
 *      std v115 (v138[PV], v137[PV], v136[C], v135[PV], v134[PV], v133[PV], v132[PR1]) =
 *         {RK_ESCAPE 1, (L)v122} -> v155
 *         v136 (v136, v135, v134, v133, v155)
 *
 *      std v122 (v145[PV], v144[PV], v143[C], v142[PV], v141[PV], v140[PV], v139[PV]) =
 *         (L)v129 (v139, (I63t)0, v143, v142, v141, v140)
 *
 *      known_chk v129 (v151[PV], v150[PV], v149[C], v148[PV], v147[PV], v146[PV]) =
 *         if boxed(v151) [v105] then
 *            v151.0 -> v152[PV]
 *            v151.1 -> v153[PV]
 *            {v152, v150} -> v154
 *            (L)v129 (v153, v154, v149, v148, v147, v146)
 *         else
 *            v149 (v149, v148, v147, v146, v150)
 * ***********************************************
 *
 * The equivalent CFG IR (with types omitted) is
 *
 * ***********************************************
 *      FUN v130 (v131, v31, v111, v112, v113, v114, v53) =
 *         ALLOC(RECORD 0x80, [LABEL v155]) -> v156
 *         ALLOC(RECORD 0x80, [VAR v156]) -> v157
 *           THROW (VAR v111) (VAR v111, VAR v112, VAR v113, VAR v114, VAR v157)
 *
 *      FUN v115 (v138[PV], v137, v136, v135, v134, v133, v132) =
 *         ALLOC(RECORD 0x80, [LABEL v122]) -> v155
 *           THROW (VAR v136) (VAR v136, VAR v135, VAR v134, VAR v133, VAR v155)
 *
 *      FUN v122 (v145, v144, v143, v142, v141, v140, v139) =
 *         GOTO v129 (VAR v139, NUM(1:i64), VAR v143, VAR v142, VAR v141, VAR v140)
 *
 *      FRAG v129 (v151, v150, v149, v148, v147, v146) =
 *         CHK_GC(NONE,
 *           IF IEQL(ANDB(v151, NUM(1:i64)), NUM(0:i64)) THEN
 *             ALLOC(RECORD 0x100, [SELECT(0, VAR v151), SELECT(1, VAR 151)]) -> v154
 *               GOTO v129 (VAR v153, VAR v154, VAR v149, VAR v148, VAR v147, VAR v146)
 *           ELSE
 *            THROW (VAR v149) (VAR v149, VAR v148, VAR v147, VAR v146, VAR v150)
 * ***********************************************
 *)

structure Ex2 =
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
      fun select (idx, arg) = C.SELECT{idx=idx, arg=arg}
      fun pureOp (oper, args) = C.PURE{oper=P.PURE_ARITH{oper=oper, sz=64}, args=args}
      fun attrs bp = { (* cluster attrs *)
	      alignHP = 8, needsBasePtr = bp, hasTrapArith = false, hasRCC = false
	    }
      val unkProb = 0

      val fn130 = C.Cluster{
	      attrs = attrs true,
	      frags = [
	        C.Frag{
		  kind = C.STD_FUN,
		  lab = v 130,
		  params = mkParams [
		      (v 131, C.PTRt), (v 31, C.PTRt), (v 111, C.LABt), (v 112, C.PTRt),
		      (v 113, C.PTRt), (v 114, C.PTRt), (v 53, C.PTRt)
		    ],
		  body = record ([LAB 115], v 156,
		    record ([V 156], v 157,
		      C.THROW (V 111,
			[V 111, V 112, V 113, V 114, V 157],
			[C.LABt, C.PTRt, C.PTRt, C.PTRt, C.PTRt])))
		}]
	    }
      val fn115 = C.Cluster{
	      attrs = attrs true,
	      frags = [
	        C.Frag{
		  kind = C.STD_FUN,
		  lab = v 115,
		  params = mkParams [
		      (v 138, C.PTRt), (v 137, C.PTRt), (v 136, C.LABt),
		      (v 135, C.PTRt), (v 134, C.PTRt), (v 133, C.PTRt), (v 132, C.PTRt)
		    ],
		  body = record ([LAB 122], v 155,
		    C.THROW (V 136,
		      [V 136, V 135, V 134, V 133, V 155],
		      [C.PTRt, C.PTRt, C.PTRt, C.PTRt, C.PTRt]))
		}]
	    }
      val fn122 = C.Cluster{
	      attrs = attrs false,
	      frags = [
	        C.Frag{
		  kind = C.STD_FUN,
		  lab = v 122,
		  params = mkParams [
		      (v 145, C.PTRt), (v 144, C.PTRt), (v 143, C.LABt), (v 142, C.PTRt),
		      (v 141, C.PTRt), (v 140, C.PTRt), (v 139, C.PTRt)
		    ],
		  body = C.GOTO (v 129, [V 139, num 1, V 143, V 142, V 141, V 140])
		},
		C.Frag{
		  kind = C.INTERNAL,
		  lab = v 129,
		  params = mkParams [
		      (v 151, C.PTRt), (v 150, C.PTRt), (v 149, C.LABt),
		      (v 148, C.PTRt), (v 147, C.PTRt), (v 146, C.PTRt)
		    ],
		  body = C.BRANCH(
		    P.CMP{oper=P.EQL, signed=false, sz=64},
		    [pureOp(P.ANDB, [V 151, num 1]), num 0],
		    unkProb,
		    (* then *)
		      record([select(0, V 151), V 150], v 154,
			C.GOTO(v 129, [select(1, V 151), V 154, V 149, V 148, V 147, V 146])),
		    (* else *)
		      C.THROW(V 149,
			[V 149, V 148, V 147, V 146, V 150],
			[C.LABt, C.PTRt, C.PTRt, C.PTRt, C.PTRt]))
		}]
	    }
    in
    val cu = {srcFile = "rev.sml", entry = fn130, fns = [fn115, fn122]}
    end (* local *)

  end
