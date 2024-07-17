#include "param.h"
#include "riscv.h"

extern int main();
extern void ex(void);

void setup(void) {
  unsigned long x = r_mstatus();
  x &= ~MSTATUS_MPP_MASK;
  x |= MSTATUS_MPP_U;
  w_mstatus(x);

  w_mstatus(r_mstatus() | MSTATUS_MIE);

  w_mie(r_mie() | MIE_MSIE);

  w_mtvec((uint64)ex);

  // set M exception program counter to main, for mret.
  // requires gcc -mcmodel=medany
  w_mepc((uint64)main);

  w_pmpaddr0(0x7FFFFFFF);
  w_pmpcfg0(0x0);

  w_pmpaddr1(0x800FFFFF);
  w_pmpcfg1(0x0);

  w_pmpaddr2(0x80FFFFFF);
  w_pmpcfg2(0xf);

  asm volatile("mret");
}
