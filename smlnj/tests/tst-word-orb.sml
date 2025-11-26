(* tst-word-orb.sml *)

fun f1 (a : word, b) = Word.orb(a, b)
fun f2 (a : word) = Word.orb(a, 0w17)
fun f3 (b : word) = Word.orb(0w42, b);
