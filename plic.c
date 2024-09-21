#include "plic.h"
#include "hardware.h"
#include "kerneldef.h"
#include "riscv.h"
#include "types.h"
#include "virtio.h"

void plic_init(void) {
  *(uint32 *)(PLIC + UART0_IRQ * 4) = 1;

  *(uint32 *)(PLIC + VIRTIO0_IRQ * 4) = 1;
}

void plic_hartinit(void) {

  int hart = cpuid();

  if (hart == 0) {
    *(uint32 *)PLIC_MENABLE(hart) = (1 << UART0_IRQ) | (1 << VIRTIO0_IRQ);
  } else {
    *(uint32 *)PLIC_MENABLE(hart) = (1 << UART0_IRQ);
  }

  *(uint32 *)PLIC_MPRIORITY(hart) = 0;

  w_mie(r_mie() | MIE_MEIE);
}

uint64 plic_claim(void) {
  int hart = cpuid();
  int irq = *(uint32 *)PLIC_MCLAIM(hart);
  return irq;
}

void plic_complete(uint64 irq) {
  int hart = cpuid();
  *(uint32 *)PLIC_MCLAIM(hart) = irq;
}
