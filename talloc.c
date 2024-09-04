#include "kernel.h"
#include "types.h"

#define PGSIZE 4096
#define KERNBASE 0x80000000L
#define PHYSTOP (KERNBASE + 128 * 1024 * 1024)
#define PGROUNDUP(sz) (((sz) + PGSIZE - 1) & ~(PGSIZE - 1))
#define PGROUNDDOWN(a) (((a)) & ~(PGSIZE - 1))

extern uint8 end[];

struct run {
  struct run *next;
};

struct {
  struct run *freelist;
} tmem;

void tmemcpy(uint8 *source, uint8 *dest, int n) {
  int i;
  for (int i = 0; i < n; i++) {
    *(dest + i) = *(source + i);
  }
}

void tmemset(uint8 *dest, uint8 val, uint32 len) {
  uint8 *temp = (uint8 *)dest;
  for (; len != 0; len--)
    *temp++ = val;
}

void tfree(void *pa) {
  struct run *r;

  if (((uint64)pa % PGSIZE) != 0 || (uint8 *)pa < end ||
      (uint64)pa >= PHYSTOP) {
    // printastring("ERROR");
    return;
  }

  tmemset(pa, 1, PGSIZE);

  r = (struct run *)pa;

  r->next = tmem.freelist;
  tmem.freelist = r;
}

void freerange(void *pa_start, void *pa_end) {
  uint8 *p;
  p = (uint8 *)PGROUNDUP((uint64)pa_start);
  for (; p + PGSIZE <= (uint8 *)pa_end; p += PGSIZE) {
    tfree(p);
  }
}

void tinit() { freerange(end, (void *)PHYSTOP); }

// Allocate 4 kb page of physical memory.
void *talloc(void) {
  struct run *r;
  r = tmem.freelist;

  if (r) {
    tmem.freelist = r->next;
  }

  if (r) {
    tmemset((uint8 *)r, 5, PGSIZE);
  }

  return (void *)r;
}
