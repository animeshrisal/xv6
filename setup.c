#include "display.h"
#include "plic.h"
#include "riscv.h"
#include "tprintf.h"
#include "trap.h"
#include "types.h"
#include "uart.h"

extern int main(void);
extern void ex(void);
extern void printstring(char *);
extern void printhex(uint64);

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
  w_pmpaddr0(0x3fffffffffffffULL);
  w_pmpcfg0(0xf);

  uart_init();
  tprintf("Starting xv6! \n");
  proc_init();
  virtio_gpu_init();
  virtio_gpu_queue_start();
  virtio_gpu_intr();

  plic_init();
  clock_init();

  w_mepc((uint64)0);
  tprintf("WRRRRRYYYYY!");
  //  return to previous mode
  asm volatile("mret");
}
