#include "proc.h"
#include <elf.h>
#include <fs.h>

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif


size_t ramdisk_read(void *buf, size_t offset, size_t len);

static uintptr_t loader(PCB *pcb, const char *filename) {
  // TODO();
  int fd = fs_open(filename, 0, 0);
  int ehdr_sz = sizeof(Elf_Ehdr);
  Elf_Ehdr ehdr;
  Elf_Phdr phdr;
  fs_direct_read(fd, &ehdr, 0, ehdr_sz);
  for(int i=0; i<ehdr.e_phnum; i++) {
    fs_direct_read(fd, &phdr, ehdr.e_phoff + i*ehdr.e_phentsize, ehdr.e_phentsize);
    if(phdr.p_type != PT_LOAD) continue;
    fs_direct_read(fd, (void*)(phdr.p_vaddr), phdr.p_offset, phdr.p_filesz);
    // printf("phdr vaddr: %d, offset: %d, filesz: %d, memsz: %d\n", phdr.p_vaddr, phdr.p_offset, phdr.p_filesz, phdr.p_memsz);
    memset((void*)(phdr.p_vaddr + phdr.p_filesz), 0, phdr.p_memsz - phdr.p_filesz);
  }  
  return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);

  Log("Jump to entry = %x", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
