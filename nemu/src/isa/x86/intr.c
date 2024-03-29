#include "rtl/rtl.h"
#include "cpu/exec.h"

#define IRQ_TIMER 32

void raise_intr(uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  assert((NO + 1) * 8 <= cpu.idtr.limit);
  rtl_push((rtlreg_t *)&cpu.eflags.value);
  rtl_push((rtlreg_t *)&cpu.cs);
  rtl_push((rtlreg_t *)&ret_addr);
  if(NO==IRQ_TIMER)
    cpu.eflags.IF = 0;
  uint32_t low = vaddr_read(cpu.idtr.base + 8 * NO, 4);
  uint32_t high = vaddr_read(cpu.idtr.base + 8 * NO + 4, 4);
  uint32_t offset = (low & 0x0000ffff) | (high & 0xffff0000);
  rtl_j(offset);
}

bool isa_query_intr(void) {
  if (cpu.INTR && cpu.eflags.IF) {
    cpu.INTR = false;
    raise_intr(IRQ_TIMER, cpu.pc);
    return true;
  }
  return false;
}
