# POSIX message queues for OCaml

The ocaml-mqueue library provides [POSIX message queue](http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/mqueue.h.html) bindings for OCaml.

This library has been tested on Linux, but QNX, Solaris etc. should work, too.
Mac OS X on the other hand does currently NOT provide POSIX message queues (as of OS X 10.10).

The [API of ocaml-mqueue](http://mqueue.forge.ocamlcore.org/doc/) is online at the [OCaml Forge](https://forge.ocamlcore.org/).

Here is an example program that opens a queue, sends a message and then receives it's own message again:
```
open Rresult
open Mqueue

let name = "/myqueue"

let _ =
  let rc =
    (* open the queue, creating it if it does not exist *)
    (mq_open name [O_RDWR; O_CREAT] 0o644 {mq_flags=0; mq_maxmsg=5; mq_msgsize=32; mq_curmsgs=0}) >>=

    (* send a message *)
    (fun q -> mq_send q {payload="hello ocaml-mqueue!"; priority=23}) >>=

    (* receive a message *)
    (fun q -> mq_receive q 32)
  in

  (* delete the queue afterwards *)
  let _ = mq_unlink name in

  (* handle the result *)
  match rc with
  | Rresult.Ok msg -> print_endline msg.payload
  | Rresult.Error (`EUnix errno) -> print_endline (Unix.error_message errno)
```

The source code of mqueue is available under the MIT license.

This library is originally written by [Markus Weissmann](http://www.mweissmann.de/)

