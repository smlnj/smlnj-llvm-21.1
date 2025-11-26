(* tst-word-mod.sml *)

fun f1 (a : word, b) = a mod b
fun f2 (a : word) = a mod 0w17
fun f3 (b : word) = 0w42 mod b;
