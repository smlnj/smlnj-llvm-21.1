(* ex03.sml
 *
 * COPYRIGHT (c) 2025 The Fellowship of SML/NJ (https://smlnj.org)
 * All rights reserved.
 *
 * Hand-crafted CFG for the function
 *
 *      fun madd (a : real, b, c) = a * b + c;
 *
 * The generated first-order CPS is:
 *
 * ***********************************************
 *      std v80 (v81[PV], v44[PV], v69[C], v70[PV], v71[PV], v72[PV], v57[PR2]) =
 *         {RK_ESCAPE 1, (L)v73} -> v91
 *         {v91} -> v92
 *         v69(v69, v70, v71, v72, v92)
 *
 *      std v73 (v90[PV], v89[PV], v88[C], v87[PV], v86[PV], v85[PV], v84[R64], v83[R64], v82[R64]) =
 *         mulf64(v84, v83) -> v46[R64]
 *         addf64(v46, v82) -> v45[R64]
 *         v88(v88, v87, v86, v85, v45)
 * ***********************************************
 *
 * The equivalent CFG IR (with types omitted) is
 *
 * ***********************************************
 *      FUN v80 (v81, v44, v69, v70, v71, v72, v57) =
 *         ALLOC(RECORD 0x80, [LABEL v73]) -> v91
 *         ALLOC(RECORD 0x80, [VAR 91]) -> v92
 *           THROW (VAR v69) (VAR v69, VAR v70, VAR v71, VAR v72, VAR v92)
 *
 *      FUN v73 (v90, v89, v88, v87, v86, v85, v84, v83, v82) =
 *         THROW (VAR v88) (VAR v88, VAR v87, VAR v86, VAR v85, FADD(FMUL(VAR v84, VAR v83), VAR v82))
 * ***********************************************
 *)

structure Ex3 =
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
	    val fldTys = List.map (fn _ => {kind = P.FLT, sz = 64}) flds
	    in
	      C.ALLOC(P.RAW_RECORD{align = 8, desc = desc, fields=fldTys}, flds, x, k)
	    end
      fun rawSelect (i, v) = C.PURE{
	      oper = P.PURE_RAW_SUBSCRIPT{kind=P.FLT, sz=64},
	      args = [v, num i]
	    }
      fun pureOp (oper, args) = C.PURE{oper=P.PURE_ARITH{oper=oper, sz=64}, args=args}
      fun gcTest (n, stm1, stm2) =
	    C.BRANCH(P.LIMIT(Word.fromInt n), [], 1, stm1, stm2)
      fun attrs bp = { (* cluster attrs *)
	      alignHP = 8, needsBasePtr = bp, hasTrapArith = false, hasRCC = false
	    }
      val unkProb = 0

      val fn80 = C.Cluster{
	      attrs = attrs true,
	      frags = [
	        C.Frag{
		  kind = C.STD_FUN,
		  lab = v 80,
		  params = mkParams [
		      (v 81, C.PTRt), (v 44, C.PTRt), (v 69, C.LABt), (v 70, C.PTRt),
		      (v 71, C.PTRt), (v 72, C.PTRt), (v 57, C.PTRt)
		    ],
		  body = gcTest (0,
		    (* THEN *)
		      C.APPLY (LAB 100,
			[V 81, V 44, V 69, V 70, V 71, V 72, V 57],
			[C.PTRt, C.PTRt, C.LABt, C.PTRt, C.PTRt, C.PTRt, C.PTRt]),
		    (* ELSE *)
		      record ([LAB 73], v 91,
		      record ([V 91], v 92,
			C.THROW (V 69,
			  [V 69, V 70, V 71, V 72, V 92],
			  [C.LABt, C.PTRt, C.PTRt, C.PTRt, C.PTRt]))))
		}]
	    }
    (* the standard GC fragment *)
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
      val fn73 = C.Cluster{
	      attrs = attrs false,
	      frags = [
	        C.Frag{
		  kind = C.STD_FUN,
		  lab = v 73,
		  params = mkParams [
		      (v 90, C.PTRt), (v 89, C.PTRt), (v 88, C.LABt),
		      (v 87, C.PTRt), (v 86, C.PTRt), (v 85, C.PTRt),
		      (v 84, C.FLTt{sz=64}), (v 83, C.FLTt{sz=64}), (v 82, C.FLTt{sz=64})
		    ],
		  body = gcTest (0,
		    (* THEN *)
		      C.APPLY (LAB 200,
			[V 90, V 89, V 88, V 87, V 86, V 85, V 84, V 83, V 82],
			[C.PTRt, C.PTRt, C.LABt, C.PTRt, C.PTRt, C.PTRt,
		          C.FLTt{sz=64}, C.FLTt{sz=64}, C.FLTt{sz=64}]),
		    (* ELSE *)
		      C.THROW (V 88, [
			    V 88, V 87, V 86, V 85,
			    pureOp(P.FADD, [pureOp(P.FMUL, [V 84, V 83]), V 82])
			  ],
			[C.PTRt, C.PTRt, C.PTRt, C.PTRt, C.FLTt{sz=64}]))
		}]
	    }
    (* the specialized GC fragment *)
      val fn200 = C.Cluster{
	      attrs = attrs false,
	      frags = [
	        C.Frag{
		  kind = C.STD_FUN,
		  lab = v 200,
		  params = mkParams [
		      (v 201, C.PTRt), (v 202, C.PTRt), (v 203, C.LABt),
		      (v 204, C.PTRt), (v 205, C.PTRt), (v 206, C.PTRt),
		      (v 207, C.FLTt{sz=64}), (v 208, C.FLTt{sz=64}),
		      (v 209, C.FLTt{sz=64})
		    ],
		  body =
		    rawRecord([V 207, V 208, V 209], v 210,
		    C.CALLGC(
		      [V 201, V 202, V 203, V 204, V 205, V 206, V 210],
		      [v 211, v 212, v 213, v 214, v 215, v 216, v 220],
		      C.APPLY(V 211,
			[V 211, V 212, V 213, V 214, V 215, V 216,
			    rawSelect(0, V 220), rawSelect(1, V 220), rawSelect(2, V 220)
			  ],
			[C.PTRt, C.PTRt, C.PTRt, C.PTRt, C.PTRt, C.PTRt,
			    C.FLTt{sz=64}, C.FLTt{sz=64}, C.FLTt{sz=64}
			  ])))
		}]
	    }
    in
    val cu = {srcFile = "fmadd.sml", entry = fn80, fns = [fn100, fn73, fn200]}
    end (* local *)

  end
