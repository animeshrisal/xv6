#include "uart.h"

void tprintf(const char *fmt) {
  while (*fmt) {
    uartputc_sync(*fmt);
    fmt++;
  }
}
