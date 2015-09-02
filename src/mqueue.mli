(*_ $Id: $
Copyright (c) 2015 Markus W. Weissmann <markus.weissmann@in.tum.de>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*)

(** POSIX message queue *)

type t
(** Type of a POSIX message queue *)

type flag =
  | O_RDONLY
  | O_WRONLY
  | O_RDWR
  | O_NONBLOCK
  | O_CREAT
  | O_EXCL
(** Flags for opening a new queue *)

type mq_attr = {
  mq_flags : int;
  mq_maxmsg : int;
  mq_msgsize : int;
  mq_curmsgs : int
}
(** attributes of a queue *)

type message = {
  payload : Bytes.t;
  priority : int;
}
(** type of a message with payload and priority; priority is a non-negative integer with system-specific upper bound -- but at least 31 *)

val mq_open : string -> flag list -> Unix.file_perm -> mq_attr -> t
(** open a POSIX message queue; [mq_open p fs perm attr] opens the message queue
  of name [p] with the given flags [fs], permissions [perm] (if created) and
  queue attributes [attr].
  [mq_open "/myqueue" [O_RDWR; O_CREAT] 0x644 {.mq_flags=0; .mq_maxmsg=10; .mq_msgsize=512; .mq_cursmsgs=0]
  opens the message queue "/myqueue" for reading and writing; if the queue does
  not yet exist, it is created with the Unix permissions set to 0x644; it can
  hold as most 10 messages of size 512 bytes each. The number of current
  messages in the queue [.mq_curmsgs] is ignored by the system call.
  Queue names follow the form of "/somename", with a leading slash and at least
  one following character (none of which are slashes) with a maximum length of
  [mq_name_max ()].
*)

val mq_send : t -> message -> unit
(** [mq_send q m] sends the nessage [m] on the queue [q]; if the queue is full,
  this call will block *)

val mq_receive : t -> int -> message
(** [mq_receive q bufsiz] removes the oldest  message  with  the highest
  priority from the message queue. The [bufsiz] argument must be at least the
  maximum message size [.mq_msgsize] of the queue attributes. The returned
  message is a copy and will not be altered by subsequent calls. *)

val mq_close : t -> unit
(** close the message queue *)

val mq_unlink : string -> unit
(** delete the given message queue *)

val mq_setattr : t -> mq_attr -> mq_attr
(** [mq_setattr q attr] tries to set the attributes of the message queue [q] to
  the new attributes [attr]. The new actual attributes are returned. *)

val mq_getattr : t -> mq_attr
(** [mq_getattr q] returns the attributes of the message queue [q] *)

val mq_prio_max : unit -> int
(** [mq_prio_max ()] provides the maximum priority that can be given to a
  message; the lowest priority is [0]. *)

val mq_name_max : unit -> int
(** [mq_name_max ()] provides the maximum name length of a message queue. *)

val fd_of : t -> Unix.file_descr
(** get the Unix file descriptor of the given message queue; this can then
  be used with [Unix.select]. This operation is valid on Linux systems but
  may provide some random file descriptor on other POSIX compliant systems. *)

(*
val mq_timedreceive : t ->
val mq_timedsend : t ->
*)

