(* tst-word64-arsh.sml *)

fun f1 (a : Word64.word, b) = Word64.~>>(a, b)
fun f2 (a : Word64.word) = Word64.~>>(a, 0w17)
fun f3 b = Word64.~>>(0w42, b);
