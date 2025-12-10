(* ex06.sml
 *
 * COPYRIGHT (c) 2025 The Fellowship of SML/NJ (https://smlnj.org)
 * All rights reserved.
 *
 * Hand-crafted CFG for the function
 *
 *      datatype t = A | B | C | D | E
 *
 *	fun foo (sel, f1, f2, f3, f4, f5, x) = (case sel
 *	       of A => f1 (x, 0)
 *		| B => f2 (x, 1)
 *		| C => f3 (x, 2)
 *		| D => f4 (x, 3)
 *		| E => f5 (x, 4)
 *	      (* end case *))
 *
 * The generated first-order CPS is:
 *
 * ***********************************************
 *      std v321 (v322[PV],v233[PV],v298[C],v299[PV],v300[PV],v301[PV],v263[PR2]) =
 *         {RK_ESCAPE 1,(L)v302} -> v350
 *         {v350} -> v351
 *         v298(v298,v299,v300,v301,v351)
 ***
 *      std v302 (v329[PV],v328[PV],v327[C],v326[PV],v325[PV],v324[PV],v323[PR2]) =
 *         {RK_ESCAPE 1,(L)v309} -> v349
 *         v327(v327,v326,v325,v324,v349)
 ***
 *      std v309 (v339[PV],v338[PV],v337[C],v336[PV],v335[PV],v334[PV],v333[I],v332[FN],v331[FN],v330[PR4]) =
 *         v330.0 -> v340[FN]
 *         v330.1 -> v341[FN]
 *         v330.2 -> v342[FN]
 *         v330.3 -> v343[PV]
 *         case v333  [292] of
 *          0 =>
 *            v332.0 -> v344[FN]
 *            v344(v344,v332,v337,v336,v335,v334,v343,(I63t)0)
 *          1 =>
 *            v331.0 -> v345[FN]
 *            v345(v345,v331,v337,v336,v335,v334,v343,(I63t)1)
 *          2 =>
 *            v340.0 -> v346[FN]
 *            v346(v346,v340,v337,v336,v335,v334,v343,(I63t)2)
 *          3 =>
 *            v341.0 -> v347[FN]
 *            v347(v347,v341,v337,v336,v335,v334,v343,(I63t)3)
 *          4 =>
 *            v342.0 -> v348[FN]
 *            v348(v348,v342,v337,v336,v335,v334,v343,(I63t)4)
 *
 * ***********************************************
 *
 * The equivalent CFG IR (with types omitted) is
 *
 * ***********************************************
 *      FUN v321 (v322, v233, v298, v299, v300, v301, v263) =
 *         ALLOC(RECORD 0x80, [LABEL v302]) -> v350
 *         ALLOC(RECORD 0x80, [VAR v350]) -> v351
 *           THROW (VAR v298) (VAR v298, VAR v299, VAR v300, VAR v301, VAR v351)
 *
 *	FUN v302 (v329, v328, v327, v326, v325, v324, v323) =
 *         ALLOC(RECORD 0x80, [LABEL v309]) -> v349
 *	   THROW (VAR v327) (VAR v327, VAR v326, VAR v325, VAR v324, VAR v349)
 *
 *      FUN v309 (v339, v338, v337, v336, v335, v334, v333, v332, v331, v330) =
 *         LET SELECT(0, VAR v330) -> v340
 *         LET SELECT(1, VAR v330) -> v341
 *         LET SELECT(2, VAR v330) -> v342
 *         LET SELECT(3, VAR v330) -> v343
 *	      SWITCH (ASHR (VAR v333, NUM(1:i64)))
 *	         case 0: LET SELECT(0, VAR v332) -> v344
 *		    APPLY (VAR v344) (
 *                     VAR v344, VAR v332, VAR v337, VAR v336, VAR v335, VAR v334, VAR v343, NUM 0)
 *	         case 1: LET SELECT(0, VAR v331) -> v345
 *                  APPLY (VAR v345) (
 *                     VAR v345, VAR v331, VAR v337, VAR v336, VAR v335, VAR v334, VAR v343, NUM 1)
 *               case 2: LET SELECT(0, VAR v340) -> v346
 *		    APPLY (VAR v346) (
 *                     VAR v346, VAR v340, VAR v337, VAR v336, VAR v335, VAR v334, VAR v343, NUM 2)
 *	         case 3: LET SELECT(0, VAR v341) -> v347
 *		    APPLY (VAR v347) (
 *                     VAR v347, VAR v341, VAR v337, VAR v336, VAR v335, VAR v334, VAR v343, NUM 3)
 *	         case 4: LET SELECT(0, VAR v342) -> v348
 *		    APPLY (VAR v348) (
 *                     VAR v348, VAR v342, VAR v337, VAR v336, VAR v335, VAR v334, VAR v343, NUM 4)
 * ***********************************************
 *)

structure Ex6 =
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

      val fn321 = C.Cluster{
	      attrs = attrs true,
	      frags = [
	        C.Frag{
		  kind = C.STD_FUN,
		  lab = v 321,
		  params = mkParams [
		      (v 322, C.PTRt), (v 233, C.PTRt), (v 298, C.LABt), (v 299, C.PTRt),
		      (v 300, C.PTRt), (v 301, C.PTRt), (v 263, C.PTRt)
		    ],
		  body = record ([LAB 302], v 190,
		    record ([V 190], v 350,
		      C.THROW (V 298,
			[V 298, V 299, V 300, V 301, V 350],
			[C.LABt, C.PTRt, C.PTRt, C.PTRt, C.PTRt])))
		}]
	    }
      val fn302 = C.Cluster{
	      attrs = attrs true,
	      frags = [
	        C.Frag{
		  kind = C.STD_FUN,
		  lab = v 302,
		  params = mkParams [
		      (v 329, C.PTRt), (v 328, C.PTRt), (v 327, C.LABt),
		      (v 326, C.PTRt), (v 325, C.PTRt), (v 324, C.PTRt),
		      (v 323, C.PTRt)
		    ],
		  body = record ([LAB 309], v 349,
		    C.THROW (V 327,
		      [V 327, V 326, V 325, V 324, V 349],
		      [C.LABt, C.PTRt, C.PTRt, C.PTRt, C.PTRt]))
		}]
	    }
      fun rule (clos, f, n) = C.LET(select(0, V clos), mkParam(v f, C.LABt),
	    C.APPLY (V f,
	      [V f, V clos, V 337, V 336, V 335, V 334, V 343, num(2*n + 1)],
	      [C.LABt, C.PTRt, C.LABt, C.PTRt, C.PTRt, C.PTRt, C.PTRt, C.TAGt]))
      val fn309 = C.Cluster{
	      attrs = attrs true,
	      frags = [
	        C.Frag{
		  kind = C.STD_FUN,
		  lab = v 309,
		  params = mkParams [
		      (v 339, C.PTRt), (v 338, C.PTRt), (v 337, C.LABt),
		      (v 336, C.PTRt), (v 335, C.PTRt), (v 334, C.PTRt),
		      (v 333, C.NUMt{sz=64}), (v 332, C.LABt), (v 331, C.LABt),
		      (v 330, C.PTRt)
		    ],
		  body =
		    C.LET(select(0, V 330), mkParam(v 340, C.LABt),
		    C.LET(select(1, V 330), mkParam(v 341, C.LABt),
		    C.LET(select(2, V 330), mkParam(v 342, C.LABt),
		    C.LET(select(3, V 330), mkParam(v 343, C.LABt),
		      C.SWITCH(pureOp(P.ASHR, [V 333, num 1]), [
			  rule (332, 344, 0),
			  rule (331, 345, 1),
			  rule (340, 346, 2),
			  rule (341, 347, 3),
			  rule (342, 348, 4)
			])))))
		}]
	    }
    in
    val cu = {srcFile = "switch.sml", entry = fn321, fns = [fn302, fn309]}
    end (* local *)

  end
