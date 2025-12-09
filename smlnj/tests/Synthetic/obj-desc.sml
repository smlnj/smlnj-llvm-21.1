(* obj-desc.sml
 *
 * COPYRIGHT (c) 2020 The Fellowship of SML/NJ (http://www.smlnj.org)
 * All rights reserved.
 *)

structure ObjDesc : sig

    type desc = IntInf.int

    val record : int -> desc
    val rawRecord : int -> desc

  end = struct

    structure II = IntInf

    type desc = II.int

    type tag = II.int

  (* taken from runtime/tags.h *)
    val tagWidth = 0w7          (* 5 minor tag bits plus 2 major tag bits *)

    val bitsPerWord = Word.fromInt 64

  (* one greater than the maximum length field value (sign should be 0) *)
    val maxLength = II.<<(1, bitsPerWord - (tagWidth+0w1))

    val powTagWidth = II.<<(1, tagWidth)

  (* tag values *)
    local
      fun mkTag t = II.orb(II.<<(t, 0w2), 2)
    in
    val tag_record      = mkTag 0
    val tag_vec_hdr     = mkTag 1
    val tag_vec_data    = tag_record
    val tag_arr_hdr     = mkTag 2
    val tag_arr_data    = mkTag 3
    val tag_ref         = tag_arr_data
    val tag_raw         = mkTag 4
    val tag_raw64       = mkTag 5
    val tag_special     = mkTag 6
    end (* local *)

  (* build a descriptor from a tag and length *)
    fun makeDesc (len, t) = II.orb(II.<<(II.fromInt len, tagWidth), t)

    fun record len = makeDesc (len, tag_record)
    fun rawRecord len = makeDesc (len, tag_raw64)

  end


