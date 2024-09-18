#include "display.h"
#include "kerneldef.h"
#include "plic.h"
#include "riscv.h"
#include "spinlock.h"
#include "talloc.h"
#include "tprintf.h"
#include "trap.h"
#include "types.h"
#include "uart.h"

// need a value thats shared between all cores
volatile static int initialized = 0;

#define NCPU 2

extern int main(void);
extern void ex(void);
extern void printstring(char *);
extern void printhex(uint64);

void test_spinlock() {}

void setup_cores() {
  if (cpuid() == 0) {

    // Use first cpu to initialize stuff
    uart_init();
    tprintf_init();
    tprintf("Setting up first CPU! \n");
    plic_init();

    plic_hartinit();
    initialized = 1;

    virtio_gpu_init();

  } else {
    while (initialized == 0)
      ;
    tprintf("Setting up second CPU! \n");
    plic_hartinit();
  };
}

__attribute__((aligned(16))) char stack0[4096 * NCPU];

void setup(void) {
  // set M Previous Privilege mode to User so mret returns to user mode.

  unsigned long x = r_mstatus();
  x &= ~MSTATUS_MPP_MASK;
  x |= MSTATUS_MPP_U;
  w_mstatus(x);

  // enable machine-mode interrupts.
  w_mstatus(r_mstatus() | MSTATUS_MIE);

  w_mtvec((uint64)ex);
  // enable software interrupts (ecall) in M mode.
  w_mie(r_mie() | MIE_MSIE);
  // set the machine-mode trap handler to jump to function "ex" when a trap
  // occurs.
  w_mepc((uint64)0);

  w_pmpaddr0(0x3fffffffffffffULL);
  w_pmpcfg0(0xf);
  int core_id = r_mhartid();
  w_tp(core_id);

  proc_init();
  setup_cores();

  //  return to previous mode
  asm volatile("mret");
}
