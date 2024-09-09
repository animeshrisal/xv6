#include "syscall.h"
#include "tprintf.h"
#include "types.h"

extern uint64 sys_uprintf(void);

static uint64 (*syscalls[])(void) = {[SYS_uprintf] sys_uprintf};

void syscall(void) {
  uint64 syscall;

  asm volatile("mv %0, a7", "=r"(syscall));

  if (syscall < 2) {
    asm volatile("mv a0, %0" : : "r"(syscalls[syscall]()))
  } else {
    tprintf("Invalid syscall")
  }
};
