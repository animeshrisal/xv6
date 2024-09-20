#ifndef KERNELDEF_H
#define KERNELDEF_H

#include "trap.h"

void syscall(registers *regs);
uint64 cpuid();

#endif
