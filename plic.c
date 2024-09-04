#include "plic.h"
#include "hardware.h"
#include "types.h"

void plic_init(void) { *(uint32 *)(PLIC + UART0_IRQ * 4) = 1; }

int plic_claim(void) {
  int irq = *(uint32 *)PLIC_MCLAIM;
  return irq;
}

void plic_complete(int irq) { *(uint32 *)PLIC_MCLAIM = irq; }
