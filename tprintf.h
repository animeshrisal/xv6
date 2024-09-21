#ifndef TPRINTF_H
#define TPRINTF_H

#include "types.h"

void tprintf(const char *fmt);
void tprinthex(uint64 hex);
void tprintint(int i);
void tprintf_init();

#endif