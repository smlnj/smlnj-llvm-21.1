(* tst-int64-quot.sml *)

fun f1 (a : Int64.int, b) = Int64.quot(a, b)
fun f2 (a : Int64.int) = Int64.quot(a, 17)
fun f3 (b : Int64.int) = Int64.quot(42, b);
