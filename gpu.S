
#include "syscall.h"

.global gpuinit
gpuinit:
  li a7, SYS_gpuinit
  ecall
  ret

.global flush
flush:
  li a7, SYS_flush 
  ecall
  ret
