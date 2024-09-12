#include "display.h"
#include "plic.h"
#include "proc.h"
#include "riscv.h"
#include "talloc.h"
#include "tprintf.h"
#include "trap.h"
#include "uart.h"

int main() {
  uart_init();
  tprintf("Starting xv6! \n");

  tinit();

  virtio_gpu_init();

  proc_init();
  plic_init();
  clock_intr();
  uart_interrupt_enable();
  proc_init();
  asm volatile("mret");
  return 0;
};
