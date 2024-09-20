#include "types.h"

struct spinlock {
  uint8 locked;

  // cpuid holding the lock
  int cpuid;
};

void initlock(struct spinlock *lock);
void acquire(struct spinlock *lock);
void release(struct spinlock *lock);
