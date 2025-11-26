(* tst-int64-rem.sml *)

fun f1 (a : Int64.int, b) = Int64.rem(a, b)
fun f2 (a : Int64.int) = Int64.rem(a, 17)
fun f3 (b : Int64.int) = Int64.rem(42, b);
