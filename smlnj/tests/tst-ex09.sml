(* tst-ex09.sml *)

fun sumVec (v : int vector) = let
      fun lp (i, s) = if (i < Word.fromInt(Vector.length v))
	    then lp (i+0w1, s + Vector.sub(v, Word.toIntX i))
	    else s
      in
	lp (0w0, 0)
      end;

