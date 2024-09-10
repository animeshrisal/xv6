#include "display.h"
#include "talloc.h"
#include "tprintf.h"
#include "uart.h"
#include "user_pong.h"

int main() {
  uart_init();

  tprintf("Starting xv6! \n");
  tinit();
  tprintf("Memory allocator initialize!\n");
  virtio_gpu_init();
  user_init();

  return 0;
};
