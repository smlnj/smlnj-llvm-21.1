(* tst-ex05.sml *)

fun try (f, h, x) = (f x) handle ex => h ex;
