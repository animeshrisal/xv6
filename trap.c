#include "trap.h"
#include "display.h"
#include "hardware.h"
#include "kerneldefs.h"
#include "plic.h"
#include "riscv.h"
#include "syscall.h"
#include "tprintf.h"
#include "types.h"
#include "uart.h"

uint64 ticks = 0;
uint64 current_pid;

void usertrap() {}

void usertrapreturn() {}

/*
void context_switch() {
  pcb[current_pid].state = READY;

  while (1) {
    current_pid = (current_pid + 1) % MAX_PROCS;
    if (pcb[current_pid].state == READY) {
      pcb[current_pid].state == RUNNING;
      break;
    }
  }

  w_mscratch(pcb[current_pid].base_address);

  pcb[current_pid].state = RUNNING;

#define SATP_SV39 (8L << 60)
  __asm__ __volatile__("sfence.vma");
}
*/

void clock_intr() {
  int interval = 20000;
  ticks++;
  *(uint64 *)CLINT_MTIMECMP(0) = *(uint64 *)CLINT_MTIME + interval;
  tprintf("Interrupted");

  if ((ticks % 10) == 0) {
    // context_switch();
  }
}

int dev_intr() {
  uint64 mcause = r_mcause();

  if (mcause == 0x800000000000000BL) {

    int irq = plic_claim();
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
    syscall();
    return 3;
  }

  else {

    return 0;
  }
}
