(* tst-ex06.sml *)

datatype t = A | B | C | D | E

fun foo (sel, f1, f2, f3, f4, f5, x) = (case sel
       of A => f1 (x, 0)
	| B => f2 (x, 1)
	| C => f3 (x, 2)
	| D => f4 (x, 3)
	| E => f5 (x, 4)
      (* end case *));

(*
val ans = foo (B, Fn.id, Fn.id, Fn.id, Fn.id, Fn.id, 42); (* == (42, 1) *)
*)
