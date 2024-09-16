#ifndef TRAP_H
#define TRAP_H

#include "proc.h"

void usertrap();
void kerneltrap();
void clock_intr();
void clock_init();
int dev_intr(registers *regs);

#endif
