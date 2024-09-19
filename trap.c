#include "trap.h"
#include "cpu.h"
#include "display.h"
#include "hardware.h"
#include "kerneldef.h"
#include "plic.h"
#include "riscv.h"
#include "tprintf.h"
#include "types.h"
#include "uart.h"

void clock_intr() {

  struct cpu *cpu = get_cpu();
  int interval = 100000;

  cpu->ticks++;

  if (cpu->cpu_id == 1) {
    tprintf("The CPU id");
    tprinthex(cpu->cpu_id);
    tprintf("\n");
  }

  *(uint64 *)CLINT_MTIMECMP(cpu->cpu_id) = *(uint64 *)CLINT_MTIME + interval;
  if ((cpu->ticks % 10) == 0) {
    proc_intr();
  }
}

void clock_init() {
  clock_intr();
  // enable machine-mode interrupts.
  w_mstatus(r_mstatus() | MSTATUS_MIE);

  // enable machine-mode timer interrupts.
  w_mie(r_mie() | MIE_MTIE);
}

int dev_intr(registers *regs) {
  uint64 mcause = r_mcause();

  if (mcause == 0x800000000000000BL) {

    int irq = plic_claim();
    if (irq == UART0_IRQ) {
      if (cpuid() == 1) {
        tprintf("interrupt sent by hart 1");
      }
      uart_intr();

    } else if (irq == 8) {
      virtio_gpu_intr();

    } else {
      tprintf("Wrong interrupt");
    }

    if (irq) {
      plic_complete(irq);
    }

    return 1;
  } else if ((mcause & ~(1ull << 63)) == 7) {
    clock_intr();
    return 2;
  } else if ((mcause & ~(1ull << 63)) == 8) {
    syscall(regs);
    return 3;
  }

  else {
    return 0;
  }
}
