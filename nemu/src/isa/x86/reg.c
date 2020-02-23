#include "nemu.h"
#include <stdlib.h>
#include <time.h>

const char *regsl[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
const char *regsw[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char *regsb[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};

void reg_test() {
  srand(time(0));
  uint32_t sample[8];
  uint32_t pc_sample = rand();
  cpu.pc = pc_sample;

  int i;
  for (i = R_EAX; i <= R_EDI; i ++) {
    sample[i] = rand();
    reg_l(i) = sample[i];
    assert(reg_w(i) == (sample[i] & 0xffff));
  }

  assert(reg_b(R_AL) == (sample[R_EAX] & 0xff));
  assert(reg_b(R_AH) == ((sample[R_EAX] >> 8) & 0xff));
  assert(reg_b(R_BL) == (sample[R_EBX] & 0xff));
  assert(reg_b(R_BH) == ((sample[R_EBX] >> 8) & 0xff));
  assert(reg_b(R_CL) == (sample[R_ECX] & 0xff));
  assert(reg_b(R_CH) == ((sample[R_ECX] >> 8) & 0xff));
  assert(reg_b(R_DL) == (sample[R_EDX] & 0xff));
  assert(reg_b(R_DH) == ((sample[R_EDX] >> 8) & 0xff));

  assert(sample[R_EAX] == cpu.eax);
  assert(sample[R_ECX] == cpu.ecx);
  assert(sample[R_EDX] == cpu.edx);
  assert(sample[R_EBX] == cpu.ebx);
  assert(sample[R_ESP] == cpu.esp);
  assert(sample[R_EBP] == cpu.ebp);
  assert(sample[R_ESI] == cpu.esi);
  assert(sample[R_EDI] == cpu.edi);

  assert(pc_sample == cpu.pc);
}

void isa_reg_display() {
  for(int index = 0; index < 8; index++)
    printf("| %-5s\t%010X\t| %-5s\t%010X\t| %-5s\t%010X|\n", regsl[index], reg_l(index), regsw[index], reg_w(index), regsb[index], reg_b(index));
  printf("pc\t%010X\n", cpu.pc);
}

uint32_t isa_reg_str2val(const char *s, bool *success) {
  assert(s != NULL);
  if (strcmp(s, "pc") == 0) {
    return cpu.pc;
  }
  for (int i = 0; i < 8; ++i) {
    if (strcmp(reg_name(i, 4), s) == 0) {
      *success = true;
      return reg_l(i);
    }
    if (strcmp(reg_name(i, 2), s) == 0) {
      *success = true;
      return reg_w(i);
    }
    if (strcmp(reg_name(i, 1), s) == 0) {
      printf("al match\n");
      *success = true;
      return reg_b(i);
    }
  }
  *success = false;
  return 0;
}
