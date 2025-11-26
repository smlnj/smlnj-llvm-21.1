(* tst-word-div.sml *)

fun f1 (a : word, b) = a div b
fun f2 (a : word) = a div 0w17
fun f3 (b : word) = 0w42 div b;
