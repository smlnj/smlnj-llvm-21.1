(* tst-word-andb.sml *)

fun f1 (a : word, b) = Word.andb(a, b)
fun f2 (a : word) = Word.andb(a, 0w17)
fun f3 (b : word) = Word.andb(0w42, b);
