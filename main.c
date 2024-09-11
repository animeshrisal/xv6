#include "display.h"
#include "proc.h"
#include "talloc.h"
#include "tprintf.h"
#include "uart.h"

int main() {
  uart_init();
  tprintf("Starting xv6! \n");
  proc_init();
  tprintf("Page table initialization finished! \n");
  tinit();

  tprintf("Memory allocator initialize!\n");
  virtio_gpu_init();

  return 0;
};
