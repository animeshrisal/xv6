#include "param.h"
#include "riscv.h"

extern int main(void);
extern void ex(void);
extern void printstring(char *);
extern void printhex(uint64);

void setup(void) {
  unsigned long x = r_mstatus();
  x &= ~MSTATUS_MPP_MASK;
  x |= MSTATUS_MPP_U;

  w_mstatus(r_mstatus() | x);

  w_mie(r_mie() | MIE_MSIE);

  w_mtvec((uint64)ex);

  // set M exception program counter to main, for mret.
  // requires gcc -mcmodel=medany
  w_satp(0);

  w_pmpaddr0(0x3fffffffffffffULL);
  w_pmpcfg0(0xf);

  w_mepc((uint64)main);

  asm volatile("mret");
}
