(* dump.sml
 *
 * COPYRIGHT (c) 2025 The Fellowship of SML/NJ (https://smlnj.org)
 * All rights reserved.
 *
 * Dump a CFG comp_unit
 *)

structure Dump : sig

    val dump : string * CFG.comp_unit -> unit

    val dumpAll : unit -> unit

  end = struct

    val dump = ASDLFilePickle.toFile CFGFilePickle.write_comp_unit

    fun dumpAll () = List.app dump [
	    ("ex00.pkl", Ex0.cu),
	    ("ex01.pkl", Ex1.cu),
	    ("ex02.pkl", Ex2.cu),
	    ("ex03.pkl", Ex3.cu),
	    ("ex04.pkl", Ex4.cu),
	    ("ex05.pkl", Ex5.cu),
	    ("ex06.pkl", Ex6.cu),
(* ex07.sml is broken
	    ("ex07.pkl", Ex7.cu),
*)
	    ("ex08.pkl", Ex8.cu),
	    ("ex09.pkl", Ex9.cu),
	    ("ex10.pkl", Ex10.cu),
	    ("ex11.pkl", Ex11.cu)(*,
	    ("ex12.pkl", Ex12.cu)*)
	  ]

  end
