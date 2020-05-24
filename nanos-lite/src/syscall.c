#include "common.h"
#include "syscall.h"
#include "fs.h"
#include "proc.h"

void naive_uload(PCB *pcb, const char *filename);
int mm_brk(uintptr_t brk, intptr_t increment);

static inline int32_t sys_write(int fd, void *buf, size_t len) {
  if (fd==1 || fd==2) {
    for (size_t i=0; i<len; i++) {
      _putc(*(char*)(buf+i));
    }
  }
  return len;
}

static inline int32_t sys_execve(const char *pathname, char *const argv[], char *const envp[]) {
  naive_uload(NULL, pathname);
  return 0;
}

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
    case SYS_exit: sys_execve("/bin/init", NULL, NULL); break;
    // case SYS_exit: _halt(0); c->GPRx = 0; break;
    case SYS_yield: _yield(); c->GPRx = 0; break;
    case SYS_open: c->GPRx = fs_open((void *)a[1], a[2], a[3]); break;
    case SYS_read: c->GPRx = fs_read(a[1], (void *)a[2], a[3]); break;
    case SYS_write: 
      // Log("write\n");
      // if(a[1]==1||a[1]==2){
			// 	  for(int i=0;i<a[3];i++)
			// 		  _putc(*(char*)(a[2]+i));
			// 	  c->GPRx=a[3];
      // } break;
      c->GPRx = fs_write(a[1], (void *)a[2], a[3]) ; break;
    case SYS_close: c->GPRx = fs_close(a[1]); break;
    case SYS_lseek: c->GPRx = fs_lseek(a[1], a[2], a[3]); break;
    case SYS_brk: c->GPRx = mm_brk(a[1], 0); break;
    case SYS_execve: sys_execve((char*)a[1], NULL, NULL); break;
    
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
