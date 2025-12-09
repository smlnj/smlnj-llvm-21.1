(* ex4.sml
 *
 * COPYRIGHT (c) 2020 The Fellowship of SML/NJ (http://www.smlnj.org)
 * All rights reserved.
 *
 * Hand-crafted CFG for the function
 *
 *      fun fact (n : Int64.int) = if (n <= 1) then 1 else n * fact(n-1);
 *
 * The generated first-order CPS is:
 *
 * ***********************************************
 *      std v96(v97[PV],v40[PV],v76[C],v77[PV],v78[PV],v79[PV],v53[PR2]) =
 *         {RK_ESCAPE 1,(L)v80} -> v121
 *         {v121} -> v122
 *         v76(v76,v77,v78,v79,v122)
 *
 *      std v80(v104[PV],v103[PV],v102[C],v101[PV],v100[PV],v99[PV],v98[I64]) =
 *         (L)v87(v102,v101,v100,v99,v98)
 *
 *      known_chk v87(v109[C],v108[PV],v107[PV],v106[PV],v105[I64]) =
 *         if i64>(v105,(I64)1) [v68] then
 *            isub64(v105,(I64)1) -> v44[I64]
 *            {RK_RAWBLOCK 1,v105} -> v119
 *            {RK_CONT 3,v109,v108,v119} -> v120
 *            (L)v87((L)v92,v120,v107,v106,v44)
 *         else
 *            v109(v109,v108,v107,v106,(I64)1)
 *
 *      std_cont v92(v114[PV],v113[PV],v112[PV],v111[PV],v110[I64]) =
 *         v113.2 -> v115[PV]
 *         v115.0 -> v116[I64]
 *         imul64(v116,v110) -> v42[I64]
 *         v113.1 -> v117[PV]
 *         v113.0 -> v118[C]
 *         v118(v118,v117,v112,v111,v42)
 * ***********************************************
 *
 * The equivalent CFG IR (with types omitted) is
 *
 * ***********************************************
 *      FUN v96 (v97, v40, v76, v77, v78, v79, v53) =
 *         ALLOC(RECORD 0x80, [LABEL v80]) -> v121
 *         ALLOC(RECORD 0x80, [VAR v121]) -> v122
 *           THROW (VAR v76) (VAR v76, VAR v77, VAR v78, VAR v79, VAR v122)
 *
 *      FUN v80 (v104, v103, v102, v101, v100, v99, v98]) =
 *         GOTO v87 (VAR v102, VAR v101. VAR v100, VAR v99, VAR v98)
 *
 *      FRAG v87 (v109, v108, v107, v106, v105) =
 *	   CHK_GC(NONE,
 *            IF IGT(VAR v105, NUM(1:i64)) THEN
 *               ISUB(v105, NUM(1:i64)) -> v44
 *               ALLOC(RAW_RECORD(, i64), [v105]) -> v119
 *               ALLOC(RECORD , [v109, v108, v119]) -> v120
 *                 GOTO v87 (LABEL 92, VAR v120, VAR v107, VAR v106, VAR v44)
 *            ELSE
 *               THROW (VAR v109) (VAR v109, VAR v108, VAR v107, VAR v106, NUM(1:i64))
 *
 *      CONT v92 (v114, v113, v112, v111, v110) =
 *	   IMUL(RAW_SELECT(0, SELECT(2, v113)), VAR v110) -> v42
 *         LET SELECT(0, VAR v113) -> v118
 *           THROW (VAR v118) [VAR v118, SELECT(1, VAR v113), VAR v112, VAR v111, VAR v42]
 * ***********************************************
 *)

structure Ex4 =
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
      fun rawRecord (flds, x, k) = let
	    val desc = ObjDesc.rawRecord(length flds)
	    val fldTys = List.map (fn _ => {kind = P.INT, sz = 64}) flds
	    in
	      C.ALLOC(P.RAW_RECORD{align = 8, desc = desc, fields=fldTys}, flds, x, k)
	    end
      fun select (idx, arg) = C.SELECT{idx=idx, arg=arg}
      fun arith (oper, args, res, k) =
	    C.ARITH(P.ARITH{oper=oper, sz=64}, args, mkParam(res, C.NUMt{sz=64}), k)
      fun rawSelect (i, v) = C.PURE{
	      oper = P.PURE_RAW_SUBSCRIPT{kind=P.INT, sz=64},
	      args = [v, num i]
	    }
      fun attrs bp = { (* cluster attrs *)
	      alignHP = 8, needsBasePtr = bp, hasTrapArith = false, hasRCC = false
	    }
      val unkProb = 0

      val fn96 = C.Cluster{
	      attrs = attrs true,
	      frags = [
	        C.Frag{
		  kind = C.STD_FUN,
		  lab = v 96,
		  params = mkParams [
		      (v 97, C.PTRt), (v 40, C.PTRt), (v 76, C.LABt), (v 77, C.PTRt),
		      (v 78, C.PTRt), (v 79, C.PTRt), (v 53, C.PTRt)
		    ],
		  body = record ([LAB 80], v 121,
		    record ([V 121], v 122,
		      C.THROW (V 76,
			[V 76, V 77, V 78, V 79, V 122],
			[C.LABt, C.PTRt, C.PTRt, C.PTRt, C.PTRt])))
		}]
	    }
      val fn80 = C.Cluster{
	      attrs = { (* function attrs *)
	          alignHP = 8, needsBasePtr = true,
		  hasTrapArith = true, hasRCC = false
	        },
	      frags = [
	        C.Frag{
		  kind = C.STD_FUN,
		  lab = v 80,
		  params = mkParams [
		      (v 104, C.PTRt), (v 103, C.PTRt), (v 102, C.LABt),
		      (v 101, C.PTRt), (v 100, C.PTRt), (v 99, C.PTRt),
		      (v 98, C.NUMt{sz=64})
		    ],
		  body = C.GOTO (v 87, [V 102, V 101, V 100, V 99, V 98])
		},
		C.Frag{
		  kind = C.INTERNAL,
		  lab = v 87,
		  params = mkParams [
		      (v 109, C.LABt), (v 108, C.PTRt), (v 107, C.PTRt),
		      (v 106, C.PTRt), (v 105, C.NUMt{sz=64})
		    ],
		  body = C.BRANCH(
		    P.CMP{oper=P.GT, signed=true, sz=64}, [V 105, num 1],
		    unkProb,
		    (* then *)
		      arith(P.ISUB, [V 105, num 1], v 44,
		      rawRecord([V 105], v 119,
		      record([V 109, V 108, V 119], v 120,
			C.GOTO(v 87,
			  [LAB 92, V 120, V 107, V 106, V 44])))),
		    (* else *)
		      C.THROW(V 109,
			[V 109, V 108, V 107, V 106, num 1],
			[C.LABt, C.PTRt, C.PTRt, C.PTRt, C.NUMt{sz=64}]))
		}]
	    }
      val fn92 = C.Cluster{
	      attrs = attrs false,
	      frags = [
	        C.Frag{
		  kind = C.STD_CONT,
		  lab = v 92,
		  params = mkParams [
		      (v 114, C.PTRt), (v 113, C.PTRt), (v 112, C.PTRt),
		      (v 111, C.PTRt), (v 110, C.NUMt{sz=64})
		    ],
		  body =
		    arith(P.IMUL, [rawSelect(0, select(2, V 113)), V 110], v 42,
		    C.LET(select(0, V 113), mkParam(v 118, C.LABt),
		      C.THROW (V 118,
			[V 118, select(1, V 113), V 112, V 111, V 42],
			[C.LABt, C.PTRt, C.PTRt, C.PTRt, C.NUMt{sz=64}])))
		}]
	    }
    in
    val cu = {srcFile = "fact.sml", entry = fn96, fns = [fn80, fn92]}
    end (* local *)

  end
