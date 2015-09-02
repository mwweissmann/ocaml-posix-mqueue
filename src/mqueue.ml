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

type t = Unix.file_descr

type flag =
  | O_RDONLY
  | O_WRONLY
  | O_RDWR
  | O_NONBLOCK
  | O_CREAT
  | O_EXCL

type mq_attr = {
  mq_flags : int;
  mq_maxmsg : int;
  mq_msgsize : int;
  mq_curmsgs : int
}

type message = {
  payload : Bytes.t;
  priority : int;
}

external mq_open : string -> flag list -> Unix.file_perm -> mq_attr -> t = "mqueue_mq_open"
external mq_close : t -> unit = "mqueue_mq_close"
external mq_send : t -> message -> unit = "mqueue_mq_send"
external mq_receive : t -> int -> message = "mqueue_mq_receive"
external mq_unlink : string -> unit = "mqueue_mq_unlink"
external mq_getattr : t -> mq_attr = "mqueue_mq_getattr"
external mq_setattr : t -> mq_attr -> mq_attr = "mqueue_mq_setattr"
external mq_prio_max : unit -> int = "mqueue_mq_prio_max"
external mq_name_max : unit -> int = "mqueue_mq_name_max"
external fd_of : t -> Unix.file_descr = "%identity"

