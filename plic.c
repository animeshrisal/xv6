#include "plic.h"
#include "hardware.h"
#include "riscv.h"
#include "types.h"
#include "virtio.h"

void plic_init(void) {
  *(uint32 *)(PLIC + UART0_IRQ * 4) = 1;

  *(uint32 *)(PLIC + VIRTIO0_IRQ * 4) = 1;

  // set enable bits for this hart's S-mode
  // for the uart and virtio gpu.
  *(uint32 *)PLIC_MENABLE = (1 << UART0_IRQ) | (1 << 8);

  // set this hart's S-mode priority threshold to 0.
  *(uint32 *)PLIC_MPRIORITY = 0;
  w_mie(r_mie() | MIE_MEIE);
}

int plic_claim(void) {
  int irq = *(uint32 *)PLIC_MCLAIM;
  return irq;
}

void plic_complete(int irq) { *(uint32 *)PLIC_MCLAIM = irq; }
