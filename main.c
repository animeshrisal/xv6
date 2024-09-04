#include "display.h"
#include "plic.h"
#include "talloc.h"
#include "uart.h"

int main() {
  plic_init();
  uart_init();
  tinit();
  virtio_gpu_init();

  while (1) {
  }
  return 0;
};
