#ifndef  PLIC_H
#define PLIC_H

#include "types.h"

void plic_init(void);
void plic_hartinit(void);
uint64 plic_claim(void);
void plic_complete(uint64 irq);

#endif