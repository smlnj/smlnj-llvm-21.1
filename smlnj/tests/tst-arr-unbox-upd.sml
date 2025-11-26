(* test codegen for unboxed update to polymorphic array *)

fun f (arr, ix, v : int) = Array.update(arr, ix, v);
