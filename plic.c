#include "plic.h"
#include "hardware.h"
#include "kerneldef.h"
#include "riscv.h"
#include "tprintf.h"
#include "types.h"
#include "virtio.h"

void plic_init(void) {
  *(uint32 *)(PLIC + UART0_IRQ * 4) = 1;

  *(uint32 *)(PLIC + VIRTIO0_IRQ * 4) = 1;
}

void plic_hartinit(void) {

  int hart = cpuid();

  *(uint32 *)PLIC_MENABLE(hart) = (1 << UART0_IRQ) | (1 << VIRTIO0_IRQ);

  *(uint32 *)PLIC_MPRIORITY(hart) = 0;
}

int plic_claim(void) {
  int hart = cpuid();
  int irq = *(uint32 *)PLIC_MCLAIM(hart);
  return irq;
}

void plic_complete(int irq) {
  int hart = cpuid();
  *(uint32 *)PLIC_MCLAIM(hart) = irq;
}
