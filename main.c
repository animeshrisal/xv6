#include "param.h"
#include "riscv.h"

__attribute__((aligned(16))) char stack0[4096 * NCPU];

void main();

void boot(void) {
  unsigned long x = r_mstatus();
  x &= ~MSTATUS_MPP_MASK;
  x |= MSTATUS_MPP_U;
  w_mstatus(x);

  // set M exception program counter to main, for mret.
  // requires gcc -mcmodel=medany
  w_mepc((uint64)main);

  asm volatile("mret");
}
