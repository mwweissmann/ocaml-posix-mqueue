open Rresult
open Mqueue

let name = "/myqueue"

let _ =
(*
  let _ = Unix.umask 0 in
*)
  let _ = mq_unlink name in
  let rc =
    (mq_open name [O_RDWR; O_CREAT] 0o644 {mq_flags=0; mq_maxmsg=5; mq_msgsize=32; mq_curmsgs=0}) >>=
    (fun q ->
      (mq_send q {payload="hello ocaml-mqueue!"; priority=23}) >>=
      (fun () -> mq_receive q 32) >>|
      (fun msg -> print_endline msg.payload)
    )
  in
  let _ = mq_unlink name in
  let () = match rc with
    | Rresult.Ok _ -> ()
    | Rresult.Error (`EUnix errno) -> print_endline (Unix.error_message errno)
  in
  let () = Gc.full_major () in
  print_endline "test done"

