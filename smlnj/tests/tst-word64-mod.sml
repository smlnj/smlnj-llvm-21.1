(* tst-word64-mod.sml *)

fun f1 (a : Word64.word, b) = a mod b
fun f2 (a : Word64.word) = a mod 0w17
fun f3 (b : Word64.word) = 0w42 mod b;
