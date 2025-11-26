(* tst-ex04.sml *)

fun fact (n : Int64.int) = if (n <= 1) then 1 else n * fact(n-1);
