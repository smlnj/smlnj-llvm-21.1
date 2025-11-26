(* tst-word-rsh.sml *)

fun f1 (a : word, b) = Word.>>(a, b)
fun f2 (a : word) = Word.>>(a, 0w17)
fun f3 (b : word) = Word.>>(0w42, b);
