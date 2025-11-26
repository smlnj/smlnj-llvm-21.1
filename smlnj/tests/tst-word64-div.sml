(* tst-word64-div.sml *)

fun f1 (a : Word64.word, b) = a div b
fun f2 (a : Word64.word) = a div 0w17
fun f3 (b : Word64.word) = 0w42 div b;
