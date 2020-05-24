#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;
PCB* fg_pcb = &pcb[1];

void naive_uload(PCB *pcb, const char *filename);
void context_kload(PCB* pcb, void* entry);
void context_uload(PCB* pcb, const char* filename);

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite for the %dth time!", j);
    j ++;
    _yield();
  }
}

void init_proc() {
  // context_kload(&pcb[0], (void*)hello_fun);
  context_uload(&pcb[1], "/bin/bmptest");

  switch_boot_pcb();

  Log("Initializing processes...");

  // load program here
  // naive_uload(NULL, "/bin/init");
}

int cnt = 0;
_Context* schedule(_Context *prev) {
  // return NULL;
  current = &pcb[1];
  // current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  return current->cp;
  // return prev;
}
