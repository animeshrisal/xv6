#include "types.h"

struct spinlock {
  uint8 locked;

  char *name;
  struct cpu *cpu;
};

void initlock(struct spinlock *lk, char *name) {
  lk->name = name;
  lk->locked = 0;
  lk->cpu = 0;
}

void acquire(struct spinlock *lk) {}

void release(struct spinlock *lk) { lk->cpu = 0; }
