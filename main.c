#include "display.h"
#include "talloc.h"
#include "tprintf.h"
#include "uart.h"
#include "user.h"

int main() {
  uart_init();

  tprintf("Starting xv6! \n");
  tinit();
  virtio_gpu_init();
  user_init();

  return 0;
};
