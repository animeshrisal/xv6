#include "syscall.h"
#include "display.h"
#include "tprintf.h"
#include "types.h"

void syscall(void) {

  uint64 syscall;
  uint64 framebuffer;
  uint64 framebuffer2;
  asm volatile("mv %0, a7" : "=r"(syscall) : :);

  switch (syscall) {
  case SYS_gpuinit:
    framebuffer = get_framebuffer();

    tprinthex(framebuffer);
    asm volatile("mv a1, %0" : : "r"(framebuffer));
    break;
  case SYS_flush:
    transfer();
    break;
  };
}
