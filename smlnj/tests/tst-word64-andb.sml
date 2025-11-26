(* tst-word64-andb.sml *)

fun f1 (a : Word64.word, b) = Word64.andb(a, b)
fun f2 (a : Word64.word) = Word64.andb(a, 0w17)
fun f3 (b : Word64.word) = Word64.andb(0w42, b);
