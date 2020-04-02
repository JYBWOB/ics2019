#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  int i,flag=0;
  for(i=0;i<8;i++) {
    if(ref_r->gpr[i]._32 != reg_l(i)) break;
  }

  if(i!=8) flag=1;

  //if(cpu.eflags.CF != ref_r->eflags.CF) flag=1;
  //else if(cpu.eflags.OF != ref_r->eflags.OF) flag=1;
  //else if(cpu.eflags.SF != ref_r->eflags.SF) flag=1;
  //else if(cpu.eflags.ZF != ref_r->eflags.ZF) flag=1;

  if(flag==1) {
    for (i=0;i<8;i++) {
      printf("ref_r: %0x %d\n", ref_r->gpr[i]._32, ref_r->gpr[i]._32);
    }
    printf("CF: %d; OF:%d; SF:%d; ZF:%d;\n",ref_r->eflags.CF, ref_r->eflags.OF, ref_r->eflags.SF, ref_r->eflags.ZF);
    printf("CF: %d; OF:%d; SF:%d; ZF:%d;\n",cpu.eflags.CF, cpu.eflags.OF, cpu.eflags.SF, cpu.eflags.ZF);
    return false;
  }

  else
  {
    return true;
  }
}

void isa_difftest_attach(void) {
}
