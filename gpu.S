
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

.global fill_rects
fill_rects:
    li a7, SYS_fill_rects
    ecall
    ret

.global make_rect
make_rect:
    li a7, SYS_make_rect
    ecall
    ret

.global clear_frame
clear_frame:
    li a7, SYS_clear_frame
    ecall
    ret

.global set_pixel
set_pixel:
    li a7, SYS_set_pixel
    ecall
    ret

