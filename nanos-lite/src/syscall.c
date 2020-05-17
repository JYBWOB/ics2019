#include "common.h"
#include "syscall.h"

static inline int32_t sys_write(int fd, void *buf, size_t len) {
  if (fd==1 || fd==2) {
    for (size_t i=0; i<len; i++) {
      _putc(*(char*)(buf+i));
    }
  }
  return len;
}

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
    case SYS_exit: _halt(a[1]); break;
    case SYS_yield: _yield(); c->GPRx = 0; break;
    case SYS_write: 
      // Log("write\n");
      c->GPRx = sys_write(a[1], (void *)a[2], (size_t)a[3]);
      break;
    case SYS_brk: c->GPRx = 0; break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
