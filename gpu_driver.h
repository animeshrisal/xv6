#ifndef GPU_DRIVER_H
#define GPU_DRIVER_H

#include "display.h"
#include "kerneldef.h"
#include "syscall.h"
#include "types.h"
#include "user_collision.h"

void gpu_command(uint64 syscall, registers *regs);

#endif
