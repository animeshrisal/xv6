#include "hardware.h"
#include "riscv.h"
#include "types.h"

extern int main(void);
extern void ex(void);
extern void printstring(char *);
extern void printhex(uint64);

void interrupt_init(void) {
  // set desired IRQ priorities non-zero (otherwise disabled).
  *(uint32 *)(PLIC + UART0_IRQ * 4) = 1;

  // set uart's enable bit for this hart's M-mode.
  *(uint32 *)PLIC_MENABLE = (1 << UART0_IRQ);

  // set this hart's M-mode priority threshold to 0.
  *(uint32 *)PLIC_MPRIORITY = 0;

  // enable machine-mode external interrupts.
  w_mie(r_mie() | MIE_MEIE);
}

void setup(void) {
  // set M Previous Privilege mode to User so mret returns to user mode.
  unsigned long x = r_mstatus();
  x &= ~MSTATUS_MPP_MASK;
  x |= MSTATUS_MPP_U;
  w_mstatus(x);

  // enable machine-mode interrupts.
  w_mstatus(r_mstatus() | MSTATUS_MIE);

  // enable software interrupts (ecall) in M mode.
  w_mie(r_mie() | MIE_MSIE);

  // set the machine-mode trap handler to jump to function "ex" when a trap
  // occurs.
  w_mtvec((uint64)ex);

  // disable paging for now
  w_satp(0);

  w_pmpaddr0(0x3fffffffffffffULL);
  w_pmpcfg0(0xf);

  // set M Exception Program Counter to main, for mret, requires gcc
  // -mcmodel=medany
  //
  interrupt_init();
  w_mepc((uint64)main);

  // switch to user mode (configured in mstatus) and jump to address in mepc CSR
  // -> main().
  asm volatile("mret");
}

void timerinit() { w_mie(r_mie() | MSTATUS_MIE); }
