#include "trap.h"
#include "cpu.h"
#include "display.h"
#include "hardware.h"
#include "kerneldef.h"
#include "plic.h"
#include "riscv.h"
#include "types.h"
#include "uart.h"

void clock_intr() {

  struct cpu *cpu = get_cpu();
  int interval = 100000;

  cpu->ticks++;
  *(uint64 *)CLINT_MTIMECMP(cpu->cpu_id) =

      *(uint64 *)CLINT_MTIME + interval;
  if ((cpu->ticks % 10) == 0) {
    proc_intr();
  }
}

void clock_init() {
  clock_intr();
  w_mie(r_mie() | MIE_MTIE);
}

int dev_intr(registers *regs) {
  uint64 mcause = r_mcause();
  struct cpu *cpu = get_cpu();

  if (mcause == 0x800000000000000BL) {

    uint64 irq = plic_claim();
    if (irq == UART0_IRQ) {
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
