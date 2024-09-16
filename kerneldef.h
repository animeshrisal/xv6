#ifndef KERNELDEF_H
#define KERNELDEF_H

#include "trap.h"

void syscall(registers *regs);
int cpuid();

#endif
