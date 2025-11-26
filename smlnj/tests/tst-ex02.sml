(* tst-ex02.sml *)

fun rev xs = let
      fun rev' ([], xs') = xs'
        | rev' (x::xs, xs') = rev' (xs, x::xs')
      in
        rev' (xs, [])
      end;
