#include "syscall.h"
#include "types.h"

extern uint64 sys_write(void);

static uint64 (*syscalls[])(void) = {[SYS_write] sys_write}

void syscall(void) {
  uint64 syscall;

  asm volatile("mv %0, a7", "=r"(syscall));

  if (syscall < 2) {
    asm volatile("mv a0, %0" : : "r"(syscalls[syscall]()))
  } else {
    printastring("Invalid syscall")
  }
};
