#include "syscall.h"
#include "display.h"
#include "gpu_driver.h"
#include "kerneldef.h"
#include "tprintf.h"
#include "types.h"

void syscall(registers *regs) {
  uint64 syscall = regs->a7;
  uint64 framebuffer;

  switch (syscall) {
  case SYS_gpuinit:
    transfer();
    break;
  case SYS_flush:
    transfer();
    break;

  case SYS_uprintf:
    tprintf((char *)virt2phys(regs->a0));
    break;

  case SYS_uprinthex:
    tprinthex(virt2phys(regs->a0));

  case SYS_make_rect:
  case SYS_clear_frame:
    gpu_command(syscall, regs);
  };
}
