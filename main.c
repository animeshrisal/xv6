#include "display.h"
#include "talloc.h"
#include "tprintf.h"
#include "uart.h"

int main() {
  uart_init();

  tprintf("Starting xv6! \n");
  tinit();
  virtio_gpu_init();

  while (1) {
  }
  return 0;
};
