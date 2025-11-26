(* tst-ex07.sml *)

datatype t = Lf of word | Nd of t * t;

fun sum (Lf w) = w | sum (Nd(t1, t2)) = sum t1 + sum t2;
