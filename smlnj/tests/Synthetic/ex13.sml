(* ex12.sml
 *
 * COPYRIGHT (c) 2026 The Fellowship of SML/NJ (https://smlnj.org)
 * All rights reserved.
 *
 * Hand-crafted CFG to test some floating-point intrinsics
 *
 *      Real.realCeil
 *      Real.realFloor
 *      Real.realRound
 *      Real.realTrunc
 *
 * The basic shape of a cluster is:
 *
 *      {
 *        std_fun (L)v0 (v1:ptr, v2:ptr, v3:label, v4:ptr, v5:ptr, v6:ptr, v7:f64) {
 *          throw v3(v3:label, v4:ptr, v5:ptr, v6:ptr, fsqrt64(v7):f64)
 *        }
 *      }
 *)

structure Ex13 =
  struct

    local
      structure CTy = CTypes
      structure P = CFG_Prim
      structure C = CFG
      structure II = IntInf
      fun mkParam (x : LambdaVar.lvar, ty : C.ty) = {name = x, ty = ty}
      val mkParams = List.map mkParam
      fun attrs bp = { (* cluster attrs *)
	      alignHP = 8, needsBasePtr = bp, hasTrapArith = false, hasRCC = false
	    }
      val f64Ty = C.FLTt{sz=64}
      fun mkCluster (baseId, prim) = let
            fun v id = LambdaVar.fromId(baseId + id)
            fun V id = C.VAR{name = v id}
            fun LAB id = C.LABEL{name = v id}
            val result = C.PURE{
                    oper = P.PURE_ARITH{oper = prim, sz = 64},
                    args = [V 7]
                  }
            in
              C.Cluster{
                  attrs = attrs false,
                  frags = [
                      C.Frag{
                          kind = C.STD_FUN,
                          lab = v 0,
                          params = mkParams [
                              (v 1, C.PTRt), (v 2, C.PTRt), (v 3, C.LABt),
                              (v 4, C.PTRt), (v 5, C.PTRt), (v 6, C.PTRt),
                              (v 7, f64Ty)
                            ],
                          body = C.THROW(V 3,
                            [V 3, V 4, V 5, V 6, result],
                            [C.LABt, C.PTRt, C.PTRt, C.PTRt, f64Ty])
                        }
                    ]
                }
            end (* mkCluster *)
    in
    val cu = {srcFile = "ex13.sml", entry = mkCluster (100, P.FSQRT), fns = [
            mkCluster (200, P.FCEIL),
            mkCluster (300, P.FFLOOR),
            mkCluster (400, P.FROUND),
            mkCluster (500, P.FTRUNC)
          ]}
    end (* local *)

  end
