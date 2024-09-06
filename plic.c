#include "plic.h"
#include "hardware.h"
#include "types.h"
#include "virtio.h"

void plic_init(void) {
  *(uint32 *)(PLIC + UART0_IRQ * 4) = 2;
  *(uint32 *)(PLIC + 1 * 4) = 1;
  *(uint32 *)(PLIC + 2 * 4) = 1;
  *(uint32 *)(PLIC + 3 * 4) = 1;
  *(uint32 *)(PLIC + 4 * 4) = 1;
  *(uint32 *)(PLIC + 5 * 4) = 1;
  *(uint32 *)(PLIC + 6 * 4) = 1;
  *(uint32 *)(PLIC + 7 * 4) = 1;
  *(uint32 *)(PLIC + 8 * 4) = 1;

  // set enable bits for this hart's S-mode
  // for the uart and virtio gpu.
  *(uint32 *)PLIC_MENABLE = (1 << UART0_IRQ) | (1 << VIRTIO0_IRQ) | (1 << 2) |
                            (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6) |
                            (1 << 7) | (1 << 8);

  // set this hart's S-mode priority threshold to 0.
  *(uint32 *)PLIC_MPRIORITY = 0;
}

int plic_claim(void) {
  int irq = *(uint32 *)PLIC_MCLAIM;
  return irq;
}

void plic_complete(int irq) { *(uint32 *)PLIC_MCLAIM = irq; }
