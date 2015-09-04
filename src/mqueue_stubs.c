/*_ $Id: $

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
*/

#include <alloca.h>
#include <mqueue.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <limits.h>
#include <errno.h>

#include <caml/alloc.h>
#include <caml/fail.h>
#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/unixsupport.h>
#include <caml/threads.h>
#include <caml/signals.h>

static value eunix;

CAMLprim value mqueue_initialize(void) {
  CAMLparam0();
  eunix = caml_hash_variant("EUnix");
  CAMLreturn (Val_unit);
}

CAMLprim value mqueue_mq_prio_max(value val_unit) {
  CAMLparam1(val_unit);
  CAMLreturn(Val_int(MQ_PRIO_MAX));
}

CAMLprim value mqueue_mq_name_max(value val_unit) {
  CAMLparam1(val_unit);
  CAMLreturn(Val_int(NAME_MAX));
}

CAMLprim value mqueue_mq_getattr(value fd) {
  CAMLparam1(fd);
  CAMLlocal3(result, perrno, rattr);

  int rc;
  struct mq_attr attr;

  caml_release_runtime_system();
  rc = mq_getattr(Int_val(fd), &attr);
  caml_acquire_runtime_system();
  if (-1 == rc) {
    goto ERROR;
  }

  rattr = caml_alloc(4, 0);
  Store_field(rattr, 0, Val_int(attr.mq_flags));
  Store_field(rattr, 1, Val_int(attr.mq_maxmsg));
  Store_field(rattr, 2, Val_int(attr.mq_msgsize));
  Store_field(rattr, 3, Val_int(attr.mq_curmsgs));

  result = caml_alloc(1, 0); // Result.Ok
  Store_field(result, 0, rattr);
goto END;

ERROR:
  perrno = caml_alloc(2, 0);
  Store_field(perrno, 0, eunix); // `EUnix
  Store_field(perrno, 1, unix_error_of_code(errno));

  result = caml_alloc(1, 1); // Result.Error
  Store_field(result, 0, perrno);

END:
  CAMLreturn(result);
}

CAMLprim value mqueue_mq_setattr(value fd, value attr) {
  CAMLparam2(fd, attr);
  CAMLlocal3(result, perrno, rattr);

  int rc;
  struct mq_attr attr_new;
  struct mq_attr attr_old;

  attr_new.mq_flags = Int_val(Field(attr, 0));
  attr_new.mq_maxmsg = Int_val(Field(attr, 1));
  attr_new.mq_msgsize = Int_val(Field(attr, 2));
  attr_new.mq_curmsgs = Int_val(Field(attr, 3));

  caml_release_runtime_system();
  rc = mq_setattr(Int_val(fd), &attr_new, &attr_old);
  caml_acquire_runtime_system();
  if (-1 == rc) {
    goto ERROR;
  }
  
  rattr = caml_alloc(4, 0);
  Store_field(rattr, 0, Val_int(attr_old.mq_flags));
  Store_field(rattr, 1, Val_int(attr_old.mq_maxmsg));
  Store_field(rattr, 2, Val_int(attr_old.mq_msgsize));
  Store_field(rattr, 3, Val_int(attr_old.mq_curmsgs));

  result = caml_alloc(1, 0); // Result.Ok
  Store_field(result, 0, rattr);
goto END;

ERROR:
  perrno = caml_alloc(2, 0);
  Store_field(perrno, 0, eunix); // `EUnix
  Store_field(perrno, 1, unix_error_of_code(errno));

  result = caml_alloc(1, 1); // Result.Error
  Store_field(result, 0, perrno);

END:
  CAMLreturn(rattr);
}

CAMLprim value mqueue_mq_close(value fd) {
  CAMLparam1(fd);
  CAMLlocal2(result, perrno);

  int rc;

  caml_release_runtime_system();
  rc = mq_close(Int_val(fd));
  caml_acquire_runtime_system();
  if (-1 == rc) {
    goto ERROR;
  }

  result = caml_alloc(1, 0); // Result.Ok
  Store_field(result, 0, Val_unit);
  goto END;

ERROR:
  perrno = caml_alloc(2, 0);
  Store_field(perrno, 0, eunix); // `EUnix
  Store_field(perrno, 1, unix_error_of_code(errno));

  result = caml_alloc(1, 1); // Result.Error
  Store_field(result, 0, perrno);

END:
  CAMLreturn(result);
}

CAMLprim value mqueue_mq_unlink(value path) {
  CAMLparam1(path);
  CAMLlocal2(result, perrno);

  int rc;
  size_t plen;
  char *buf;

  plen = caml_string_length(path);
#ifdef NOALLOCA
  if (NULL == (buf = malloc(plen + 1))) {
    caml_raise_out_of_memory();
  }
#else
  buf = alloca(plen + 1);
#endif
  memcpy(buf, String_val(path), plen);
  buf[plen] = '\0';

  caml_release_runtime_system();
  rc = mq_unlink(buf);
  caml_acquire_runtime_system();

  if (-1 == rc) {
    goto ERROR;
  }
#ifdef NOALLOCA
  free(buf);
#endif

  result = caml_alloc(1, 0); // Result.Ok
  Store_field(result, 0, Val_unit);
  goto END;

ERROR:
  perrno = caml_alloc(2, 0);
  Store_field(perrno, 0, eunix); // `EUnix
  Store_field(perrno, 1, unix_error_of_code(errno));

  result = caml_alloc(1, 1); // Result.Error
  Store_field(result, 0, perrno);

END:
  CAMLreturn(result);
}

static int open_flag_table[6] = {
  O_RDONLY, O_WRONLY, O_RDWR, O_NONBLOCK, O_CREAT, O_EXCL
};

CAMLprim value mqueue_mq_open(value path, value flags, value perm, value attr) {
  CAMLparam4(path, flags, perm, attr);
  CAMLlocal2(result, perrno);

  int fd;
  char *p;
  int cv_flags;
  size_t plen;
  struct mq_attr a;
  mode_t mode;

  plen = caml_string_length(path);
#ifdef NOALLOCA
  if (NULL == (p = malloc(plen + 1))) {
    caml_raise_out_of_memory();
  }
#else
  p = alloca(plen + 1);
#endif
  memcpy(p, String_val(path), plen);
  p[plen] = '\0';

  cv_flags = convert_flag_list(flags, open_flag_table);

  a.mq_flags = Int_val(Field(attr, 0));
  a.mq_maxmsg = Int_val(Field(attr, 1));
  a.mq_msgsize = Int_val(Field(attr, 2));
  a.mq_curmsgs = Int_val(Field(attr, 3));

  mode = Int_val(perm);

  caml_release_runtime_system();
  fd = mq_open(p, cv_flags, mode, &a);
#ifdef NOALLOCA
  free(p);
#endif
  caml_acquire_runtime_system();

  if (-1 == fd) {
    goto ERROR;
  }

  result = caml_alloc(1, 0); // Result.Ok
  Store_field(result, 0, Val_int(fd));
  goto END;

ERROR:
  perrno = caml_alloc(2, 0);
  Store_field(perrno, 0, eunix); // `EUnix
  Store_field(perrno, 1, unix_error_of_code(errno));

  result = caml_alloc(1, 1); // Result.Error
  Store_field(result, 0, perrno);

END:
  CAMLreturn(result);
}

CAMLprim value mqueue_mq_send(value fd, value message) {
  CAMLparam2(fd, message);
  CAMLlocal2(result, perrno);

  int rc;
  size_t msg_len;
  unsigned int p;
  char *buf;
  mqd_t q;

  p = Int_val(Field(message, 1));

  msg_len = caml_string_length(Field(message, 0));
#ifdef NOALLOCA
  if (NULL == (buf = malloc(msg_len))) {
    caml_raise_out_of_memory();
  }
#else
  buf = alloca(msg_len);
#endif
  memcpy(buf, String_val(Field(message, 0)), msg_len);
  q = Int_val(fd);

  caml_release_runtime_system();
  rc = mq_send(q, buf, msg_len, p);

#ifdef NOALLOCA
  free(buf);
#endif
  caml_acquire_runtime_system();

  if (-1 == rc) {
    goto ERROR;
  }

  result = caml_alloc(1, 0); // Result.Ok
  Store_field(result, 0, fd);
  goto END;

ERROR:
  perrno = caml_alloc(2, 0);
  Store_field(perrno, 0, eunix); // `EUnix
  Store_field(perrno, 1, unix_error_of_code(errno));

  result = caml_alloc(1, 1); // Result.Error
  Store_field(result, 0, perrno);

END:
  CAMLreturn(result);
}

CAMLprim value mqueue_mq_timedsend(value fd, value message, value timeout) {
  CAMLparam3(fd, message, timeout);
  CAMLlocal2(result, perrno);

  int rc;
  size_t msg_len;
  unsigned int p;
  char *buf;
  mqd_t q;
  struct timespec time;

  p = Int_val(Field(message, 1));
  time.tv_sec = Int_val(Field(timeout, 0));
  time.tv_nsec = Int_val(Field(timeout, 1));

  msg_len = caml_string_length(Field(message, 0));
#ifdef NOALLOCA
  if (NULL == (buf = malloc(msg_len))) {
    caml_raise_out_of_memory();
  }
#else
  buf = alloca(msg_len);
#endif
  memcpy(buf, String_val(Field(message, 0)), msg_len);
  q = Int_val(fd);

  caml_release_runtime_system();
  rc = mq_timedsend(q, buf, msg_len, p, &time);

#ifdef NOALLOCA
  free(buf);
#endif
  caml_acquire_runtime_system();

  if (-1 == rc) {
    goto ERROR;
  }

  result = caml_alloc(1, 0); // Result.Ok
  Store_field(result, 0, fd);
  goto END;

ERROR:
  perrno = caml_alloc(2, 0);
  Store_field(perrno, 0, eunix); // `EUnix
  Store_field(perrno, 1, unix_error_of_code(errno));

  result = caml_alloc(1, 1); // Result.Error
  Store_field(result, 0, perrno);

END:
  CAMLreturn(result);
}

CAMLprim value mqueue_mq_receive(value fd, value size) {
  CAMLparam2(fd, size);
  CAMLlocal4(message, payload, result, perrno);

  size_t bufsiz;
  mqd_t q;
  char *buf;
  ssize_t msglen;
  unsigned int prio;

  bufsiz = Int_val(size);
  q = Int_val(fd);

#ifdef NOALLOCA
  if (NULL == (buf = malloc(bufsiz))) {
    caml_raise_out_of_memory();
  }
#else
  buf = alloca(bufsiz);
#endif

  caml_release_runtime_system();
  msglen = mq_receive(q, buf, bufsiz, &prio);
  caml_acquire_runtime_system();

  if (-1 == msglen) {
#ifdef NOALLOCA
    free(buf);
#endif
    goto ERROR;
  }
  payload = caml_alloc_string(msglen);
  memcpy(String_val(payload), buf, msglen);

#ifdef NOALLOCA
  free(buf);
#endif

  message = caml_alloc(2, 0);
  Store_field(message, 0, payload);
  Store_field(message, 1, Val_int(prio));

  result = caml_alloc(1, 0); // Result.Ok
  Store_field(result, 0, message);
  goto END;

ERROR:
  perrno = caml_alloc(2, 0);
  Store_field(perrno, 0, eunix); // `EUnix
  Store_field(perrno, 1, unix_error_of_code(errno));

  result = caml_alloc(1, 1); // Result.Error
  Store_field(result, 0, perrno);

END:
  CAMLreturn(result);
}

CAMLprim value mqueue_mq_timedreceive(value fd, value size, value timeout) {
  CAMLparam3(fd, size, timeout);
  CAMLlocal4(message, payload, result, perrno);

  size_t bufsiz;
  mqd_t q;
  char *buf;
  ssize_t msglen;
  unsigned int prio;
  struct timespec time;

  bufsiz = Int_val(size);
  q = Int_val(fd);
  time.tv_sec = Int_val(Field(timeout, 0));
  time.tv_nsec = Int_val(Field(timeout, 1));

#ifdef NOALLOCA
  if (NULL == (buf = malloc(bufsiz))) {
    caml_raise_out_of_memory();
  }
#else
  buf = alloca(bufsiz);
#endif

  caml_release_runtime_system();
  msglen = mq_timedreceive(q, buf, bufsiz, &prio, &time);
  caml_acquire_runtime_system();

  if (-1 == msglen) {
#ifdef NOALLOCA
    free(buf);
#endif
    goto ERROR;
  }
  payload = caml_alloc_string(msglen);
  memcpy(String_val(payload), buf, msglen);

#ifdef NOALLOCA
  free(buf);
#endif

  message = caml_alloc(2, 0);
  Store_field(message, 0, payload);
  Store_field(message, 1, Val_int(prio));

  result = caml_alloc(1, 0); // Result.Ok
  Store_field(result, 0, message);
  goto END;

ERROR:
  perrno = caml_alloc(2, 0);
  Store_field(perrno, 0, eunix); // `EUnix
  Store_field(perrno, 1, unix_error_of_code(errno));

  result = caml_alloc(1, 1); // Result.Error
  Store_field(result, 0, perrno);

END:
  CAMLreturn(result);
}

/*
int      mq_notify(mqd_t, const struct sigevent * );
ssize_t  mq_timedreceive(mqd_t, char *restrict, size_t,
             unsigned *restrict, const struct timespec *restrict);
int      mq_timedsend(mqd_t, const char *, size_t, unsigned,
             const struct timespec * );
*/
