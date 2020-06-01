#include "memory.h"
#include "proc.h"

static void *pf = NULL;
extern PCB* current;

void* new_page(size_t nr_page) {
  void *p = pf;
  pf += PGSIZE * nr_page;
  assert(pf < (void *)_heap.end);
  return p;
}

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk, intptr_t increment) {
  // return 0;
  Log("new_brk:%x", brk);
  Log("current->cur_brk:%x", current->as);
  Log("current->max_brk:%x", current->max_brk);
  if(brk>current->max_brk){
    int len = brk - current->max_brk;
    while(len >0){
      void* paddr = new_page(1);
      _map(&current->as, (void*)current->max_brk, paddr, 0);
      current->max_brk += PGSIZE;
      len -= PGSIZE;
    }
  }
  return 0;
}

void init_mm() {
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  Log("free physical pages starting from %p", pf);

  _vme_init(new_page, free_page);
}
