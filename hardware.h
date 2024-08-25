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
