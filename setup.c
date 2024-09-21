#include "cpu.h"
#include "display.h"
#include "kerneldef.h"
#include "plic.h"
#include "riscv.h"
#include "spinlock.h"
#include "trap.h"
#include "types.h"
#include "uart.h"

// need a value thats shared between all cores
volatile static int initialized = 0;

#define NCPU 2

extern int main(void);
extern void ex(void);

void setup_cores() {
  if (cpuid() == 0) {
    tprintf("Setting up first CPU! \n");
    // Use first cpu to initialize stuff
    init_cpu(1, 0x81000000ULL);
    proc_init();
    uart_init();
    tprintf_init();

    plic_init();
    plic_hartinit();
    virtio_gpu_init();
    clock_init();
    initialized = 1;

  } else {
    while (initialized == 0)
      ;
    tprintf("Setting up second CPU! \n");
    init_cpu(2, 0x82000000ULL);
    proc_init();
    plic_hartinit();
    clock_init();
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

  uint64 core_id = r_mhartid();
  w_tp(core_id);

  setup_cores();

  //  return to previous mode
  asm volatile("mret");
}
