#include "kernel.h"

void *memset(void *dst, int value, int num) {
  char *cdst = (char *)dst;

  int i;

  for (i = 0; i < num; i++) {
    cdst[i] = value;
  }

  return dst;
}
