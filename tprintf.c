#include "spinlock.h"
#include "types.h"
#include "uart.h"

struct spinlock printfspinlock;

void tprintf_init() { initlock(&printfspinlock); };

void tprintf(const char *fmt) {
  acquire(&printfspinlock);

  while (*fmt) {
    uartputc_sync(*fmt);
    fmt++;
  }

  release(&printfspinlock);
}

void tprinthex(uint64 hex) {
  acquire(&printfspinlock);

  int i;
  char s;

  uartputc_sync('0');
  uartputc_sync('x');
  for (i = 60; i >= 0; i -= 4) {
    int d = ((hex >> i) & 0x0f);
    if (d < 10)
      s = d + '0';
    else
      s = d - 10 + 'a';
    uartputc_sync(s);
  }
  uartputc_sync('\n');
  release(&printfspinlock);
}
