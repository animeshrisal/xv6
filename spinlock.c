#include "spinlock.h"

void initlock(struct spinlock *lock) {
  lock->cpuid = -1;
  lock->locked = 0;
}

void acquire(struct spinlock *lock) {
  // disable_interrupts();
  while (__sync_lock_test_and_set(&lock->locked, 1) != 0)
    ;
}

void release(struct spinlock *lock) {
  lock->cpuid = -1;
  __sync_lock_release(&lock->locked);

  // enable_interrupts();
}
