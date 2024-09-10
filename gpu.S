
#include "syscall.h"

.global gpuinit
gpuinit:
  li a7, SYS_gpuinit
  ecall
  ret

.global gpu
gpu:
  li a7, SYS_gpu 
  ecall
  ret
