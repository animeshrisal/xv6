#ifndef KERNELDEF_H
#define KERNELDEF_H

#include "trap.h"

void syscall(registers *regs);
uint64 cpuid();

void tprintf(const char *fmt);
void tprinthex(uint64 hex);
void tprintint(int i);
void tprintf_init();
#endif
