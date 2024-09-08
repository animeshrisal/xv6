#include "display.h"
#include "talloc.h"
#include "tprintf.h"
#include "uart.h"

int main() {
  uart_init();

  tprintf("Starting xv6! \n");
  tinit();
  virtio_gpu_init();
  int idx = 10;
  while (1) {
    idx = (idx + 1) % 255;
    transfer(idx);
  }
  return 0;
};
