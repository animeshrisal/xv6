#include "syscall.h"

.global uprintf
uprintf:
  li a7, SYS_uprintf
  ecall
  ret

.global uprinthex
uprinthex:
  li a7, SYS_uprinthex
  ecall
  ret

