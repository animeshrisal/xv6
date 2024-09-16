#include "uart.h"
#include "hardware.h"

volatile struct uart *uart0 = (volatile struct uart *)0x10000000;

void uart_init() { uart0->IER = 0x1; };

void uart_intr(void) {
  while (1) {
    int c = uart_getc();
    if (c == -1) {
      break;
    }

    if (c == 0x100) {
      uartputc_sync('\b');
      uartputc_sync(' ');
      uartputc_sync('\b');

    } else {
      uartputc_sync(c);
    }
  }
}

int uart_getc(void) {
  if (uart0->LSR & 0x01) {
    return uart0->RBR;
  } else {
    return -1;
  }
}

void uartputc_sync(int c) { uart0->THR = c; }
