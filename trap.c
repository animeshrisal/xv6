#include "trap.h"
#include "display.h"
#include "hardware.h"
#include "plic.h"
#include "riscv.h"
#include "tprintf.h"
#include "types.h"
#include "uart.h"

uint64 ticks;

void usertrap() {}

void clock_intr() { w_mtimecmp(r_time() + 1000000); }

int dev_intr() {
  uint64 mcause = r_mcause();
  if (mcause == 0x800000000000000BL) {

    int irq = plic_claim();
    if (irq == UART0_IRQ) {
      uart_intr();
    } else if (irq == VIRTIO_IRQ) {
      tprintf("REEEE");
      virtio_gpu_intr();
    } else {
      tprintf("Wrong interrupt");
    }

    if (irq) {
      plic_complete(irq);
    }

    return 1;
  } else if (mcause == 0x8000000000000005L) {
    clock_intr();
    return 2;
  } else {
    return 0;
  }
}
