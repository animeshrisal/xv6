#include "syscalls.h"

.global uprintf
uprintf:
  li a7, SYS_urprintf
  ecall
  ret


