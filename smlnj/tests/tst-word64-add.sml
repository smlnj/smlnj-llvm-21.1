(* tst-word64-add.sml *)

fun f1 (a : Word64.word, b) = a + b
fun f2 (a : Word64.word) = a + 0w17
fun f3 (b : Word64.word) = 0w42 + b;
