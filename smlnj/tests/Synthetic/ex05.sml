(* ex05.sml
 *
 * COPYRIGHT (c) 2025 The Fellowship of SML/NJ (https://smlnj.org)
 * All rights reserved.
 *
 * Hand-crafted CFG for the function
 *
 *      fun try (f, h, x) = (f x) handle ex => h ex;
 *
 * The generated first-order CPS is:
 *
 * ***********************************************
 *      std v333(v334[PV],v263[PV],v303[C],v304[PV],v305[PV],v306[PV],v279[PR2]) =
 *         {RK_ESCAPE 1,(L)v307} -> v377
 *         {v377} -> v378
 *         v303(v303,v304,v305,v306,v378)
 *
 *      std v307(v341[PV],v340[PV],v339[C],v338[PV],v337[PV],v336[PV],v335[PR2]) =
 *         {RK_ESCAPE 1,(L)v314} -> v376
 *         v339(v339,v338,v337,v336,v376)
 *
 *      std v314(v350[PV],v349[PV],v348[C],v347[PV],v346[PV],v345[PV],v344[FN],v343[FN],v342[PV]) =
 *         gethdlr() -> v293[FN]
 *         {RK_ESCAPE 7,(L)v321,v343,v293,v348,v347,v346,v345} -> v365
 *         sethdlr(v365)
 *         {RK_CONT 3,v293,v348,v347} -> v374
 *         v344.0 -> v375[FN]
 *         v375(v375,v344,(L)v329,v374,v346,v345,v342)
 *
 *      std v321(v357[PV],v356[PV],v355[C],v354[PV],v353[PV],v352[PV],v351[PV]) =
 *         v356.2 -> v358[FN]
 *         sethdlr(v358)
 *         v356.1 -> v359[FN]
 *         v356.6 -> v360[PV]
 *         v356.5 -> v361[PV]
 *         v356.4 -> v362[PV]
 *         v356.3 -> v363[C]
 *         v359.0 -> v364[FN]
 *         v364(v364,v359,v363,v362,v361,v360,v351)
 *
 *      std_cont v329(v370[PV],v369[PV],v368[PV],v367[PV],v366[PV]) =
 *         v369.0 -> v371[FN]
 *         sethdlr(v371)
 *         v369.2 -> v372[PV]
 *         v369.1 -> v373[C]
 *         v373(v373,v372,v368,v367,v366)
 * ***********************************************
 *
 * The equivalent CFG IR (with types omitted) is
 *
 * ***********************************************
 *      FUN v333 (v334, v263, v303, v304, v305, v306, v279) =
 *         ALLOC(RECORD 0x80, [LABEL v307]) -> v377
 *         ALLOC(RECORD 0x80, [VAR v377]) -> v378
 *           THROW (VAR v303) (VAR v303, VAR v304, VAR v305, VAR v306, VAR v378)
 *
 *      FUN v307 (v341, v340, v339, v338, v337, v336, v335) =
 *         ALLOC(RECORD 0x80, [LABEL v314]) -> v376
 *	   THROW (VAR v339) (VAR v339, VAR v338, VAR v337, VAR v336, VAR v376)
 *
 *      FUN v314 (v350, v349, v348, v347, v346, v345, v344, v343, v342) =
 *	   LET GET_HDLR -> v293
 *	   ALLOC(RECORD 0x380, [
 *	         LABEL v321, VAR v343, VAR v293, VAR v348, VAR v347, VAR v346, VAR v345
 *            ]) -> v365
 *	   SET_HDLR (v365)
 *	   ALLOC(RECORD 0x180, [VAR v293, VAR v348, VAR v347]) -> v374
 *	   LET SELECT(0, VAR v344) -> v375
 *	      APPLY (VAR 375) (VAR v375, VAR v344, LABEL v329, VAR v374, VAR v346, VAR v345, VAR v342)
 *
 *      FUN v321 (v357, v356, v355, v354, v353, v352, v351) =
 *	   SET_HDLR(SELECT(2, VAR v356))
 *	   LET SELECT(0, VAR v356) -> v364
 *	      APPLY (VAR v364) (VAR v364, SELECT(1, VAR v356), SELECT(3, VAR v356),
 *		 SELECT(4, VAR v356), SELECT(5, VAR v356), SELECT(6, VAR v356), VAR v351)
 *
 *      CONT v329 (v370, v369, v368, v367, v366) =
 *	   SET_HDLR(SELECT(0, VAR v369))
 *	   LET SELECT(1, VAR v369) -> v373
 *	      THROW (VAR v373) (VAR v373, VAR v372, VAR v368, VAR v367, VAR v366)
 * ***********************************************
 *)

structure Ex5 =
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
      fun setHdlr (exp, k) = C.SETTER(P.SET_HDLR, [exp], k)
      fun rawRecord (flds, x, k) = let
	    val desc = ObjDesc.rawRecord(length flds)
	    val fldTys = List.map (fn _ => {kind = P.INT, sz = 64}) flds
	    in
	      C.ALLOC(P.RAW_RECORD{align = 8, desc = desc, fields=fldTys}, flds, x, k)
	    end
      fun attrs bp = { (* cluster attrs *)
	      alignHP = 8, needsBasePtr = bp, hasTrapArith = false, hasRCC = false
	    }
      val unkProb = 0

      val fn333 = C.Cluster{
	      attrs = attrs true,
	      frags = [
	        C.Frag{
		  kind = C.STD_FUN,
		  lab = v 333,
		  params = mkParams [
		      (v 334, C.PTRt), (v 263, C.PTRt), (v 303, C.LABt), (v 304, C.PTRt),
		      (v 305, C.PTRt), (v 306, C.PTRt), (v 279, C.PTRt)
		    ],
		  body = record ([LAB 307], v 377,
		    record ([V 377], v 378,
		      C.THROW (V 303,
			[V 303, V 304, V 305, V 306, V 378],
			[C.LABt, C.PTRt, C.PTRt, C.PTRt, C.PTRt])))
		}]
	    }
      val fn307 = C.Cluster{
	      attrs = attrs true,
	      frags = [
	        C.Frag{
		  kind = C.STD_FUN,
		  lab = v 307,
		  params = mkParams [
		      (v 341, C.PTRt), (v 340, C.PTRt), (v 339, C.LABt), (v 338, C.PTRt),
		      (v 337, C.PTRt), (v 336, C.PTRt), (v 335, C.PTRt)
		    ],
		  body = record ([LAB 314], v 376,
		    C.THROW (V 339,
		      [V 339, V 338, V 337, V 336, V 376],
		      [C.LABt, C.PTRt, C.PTRt, C.PTRt, C.PTRt]))
		}]
	    }
      val fn314 = C.Cluster{
	      attrs = attrs true,
	      frags = [
	        C.Frag{
		  kind = C.STD_FUN,
		  lab = v 314,
		  params = mkParams [
		      (v 350, C.PTRt), (v 349, C.PTRt), (v 348, C.LABt), (v 347, C.PTRt),
		      (v 346, C.PTRt), (v 345, C.PTRt), (v 344, C.LABt), (v 343, C.LABt),
		      (v 342, C.PTRt)
		    ],
		  body = C.LET(C.LOOKER{oper=P.GET_HDLR, args=[]}, mkParam(v 293, C.LABt),
		    record([LAB 321, V 343, V 293, V 348, V 347, V 346, V 345], v 365,
		    setHdlr(V 365,
		    record([V 293, V 348, V 347], v 374,
		    C.LET(select(0, V 344), mkParam(v 375, C.LABt),
		      C.APPLY(V 375,
			[V 375, V 344, LAB 329, V 374, V 346, V 345, V 342],
			[C.LABt, C.PTRt, C.LABt, C.PTRt, C.PTRt, C.PTRt, C.PTRt]))))))
		}]
	    }
      val fn321 = C.Cluster{
	      attrs = attrs false,
	      frags = [
	        C.Frag{
		  kind = C.STD_FUN,
		  lab = v 321,
		  params = mkParams [
		      (v 357, C.PTRt), (v 356, C.PTRt), (v 355, C.LABt), (v 354, C.PTRt),
		      (v 353, C.PTRt), (v 352, C.PTRt), (v 351, C.PTRt)
		    ],
		  body = setHdlr(select(2, V 356),
		    C.LET(select(0, V 356), mkParam(v 364, C.LABt),
		      C.APPLY(V 364, [
			    V 364, select(1, V 356), select(3, V 356), select(4, V 356),
			    select(5, V 356), select(6, V 356), V 351
			  ],
			[C.LABt, C.LABt, C.LABt, C.PTRt, C.PTRt, C.PTRt, C.PTRt])))
		}]
	    }
      val fn329 = C.Cluster{
	      attrs = attrs false,
	      frags = [
	        C.Frag{
		  kind = C.STD_CONT,
		  lab = v 329,
		  params = mkParams [
		      (v 370, C.PTRt), (v 369, C.PTRt), (v 368, C.PTRt), (v 367, C.PTRt),
		      (v 366, C.PTRt)
		    ],
		  body = setHdlr(select(0, V 369),
		    C.LET(select(1, V 369), mkParam(v 373, C.LABt),
		      C.THROW(V 373,
			[V 373, select(2, V 369), V 368, V 367, V 366],
			[C.LABt, C.PTRt, C.PTRt, C.PTRt, C.PTRt])))
		}]
	    }
    in
    val cu = {srcFile = "try.sml", entry = fn333, fns = [fn307, fn314, fn321, fn329]}
    end (* local *)

  end
