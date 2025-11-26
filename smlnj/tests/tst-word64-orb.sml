(* tst-word64-orb.sml *)

fun f1 (a : Word64.word, b) = Word64.orb(a, b)
fun f2 (a : Word64.word) = Word64.orb(a, 0w17)
fun f3 (b : Word64.word) = Word64.orb(0w42, b);
