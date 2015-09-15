# POSIX message queues for OCaml

The ocaml-mqueue library provides [POSIX message queue](http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/mqueue.h.html) bindings for OCaml.

This library has been tested on Linux, but QNX, Solaris etc. should work, too.
Mac OS X on the other hand does currently NOT provide POSIX message queues (as of OS X 10.10).

The [API of ocaml-mqueue](http://mqueue.forge.ocamlcore.org/doc/) is online at the [OCaml Forge](https://forge.ocamlcore.org/).

Here is an example program that opens a queue, sends a message and then receives it's own message again:
```ocaml
open Rresult
open Mqueue

let name = "/myqueue"

let _ =
  let rc =
    (* open the queue, creating it if it does not exist *)
    (mq_open name [O_RDWR; O_CREAT] 0o644 {mq_flags=0; mq_maxmsg=5; mq_msgsize=32; mq_curmsgs=0}) >>=

    (* if the queue is opened successfully... *)
    (fun mq ->
      (* .. send a message .. *)
      (mq_send mq {payload="hello ocaml-mqueue!"; priority=23}) >>=

      (* .. and receive a message .. *)
      (fun () -> mq_receive mq 32) >>|
      
      (* .. and print the message *)
      (fun msg -> print_endline msg.payload)
    )
  in

  (* remove the queue from the system  *)
  let _ = mq_unlink name in

  (* handle the result *)
  match rc with
  | Rresult.Ok () -> print_endline "done"
  | Rresult.Error (`EUnix errno) -> print_endline (Unix.error_message errno)
```

The source code of mqueue is available under the MIT license.

This library is originally written by [Markus Weissmann](http://www.mweissmann.de/)

