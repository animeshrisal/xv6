#ifndef HARDWARE_H
#define HARDWARE_H

#include "types.h"

struct uart {
  union {
    uint8 THR; // W = transmit hold register (offset 0)
    uint8 RBR; // R = receive buffer register (also offset 0)
    uint8 DLL; // R/W = divisor latch low (offset 0 when DLAB=1)
  };
  union {
    uint8 IER; // R/W = interrupt enable register (offset 1)
    uint8 DLH; // R/W = divisor latch high (offset 1 when DLAB=1)
  };
  union {
    uint8 IIR; // R = interrupt identif. reg. (offset 2)
    uint8 FCR; // W = FIFO control reg. (also offset 2)
  };
  uint8 LCR; // R/W = line control register (offset 3)
  uint8 MCR; // R/W = modem control register (offset 4)
  uint8 LSR; // R   = line status register (offset 5)
};

// core local interrupto (CLINT)
#define CLINT 0x2000000L
#define CLINT_MTIMECMP(hartid) (CLINT + 0x4000 + (hartid) * 0x8)
#define CLINT_MTIME (CLINT + 0xBFF8) // cycles since boot
#define CLINT_MSIP(hartid) (CLINT + 0x0 + (hartid) * 4)
// platform level interrupt controller (PLIC)

#define PLIC 0x0c000000L
#define PLIC_PRIORITY (PLIC + 0x0)
#define PLIC_PENDING (PLIC + 0x1000)

// Update the machine-level definitions to include `hart`
#define PLIC_MENABLE(hart) (PLIC + 0x2000 + (hart) * 0x100)
#define PLIC_MPRIORITY(hart) (PLIC + 0x200000 + (hart) * 0x2000)
#define PLIC_MCLAIM(hart) (PLIC + 0x200004 + (hart) * 0x2000)

#define UART0_IRQ 10
#define VIRTIO_IRQ 1

#define MTI 7
#define MEI 11

#endif
