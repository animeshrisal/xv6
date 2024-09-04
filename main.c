#include "display.h"
#include "plic.h"
#include "talloc.h"
#include "tprintf.h"
#include "uart.h"

int main() {
  plic_init();
  uart_init();

  tprintf("Starting xv6! \n");
  tinit();
  virtio_gpu_init();

  while (1) {
  }
  return 0;
};
