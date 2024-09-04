#include "uart.h"
#include "display.h"
#include "hardware.h"

volatile struct uart *uart0 = (volatile struct uart *)0x10000000;

char uart_buf[UART_BUF_SIZE];
uint64 uart_buf_w;
uint64 uart_buf_r;

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

  uart_start();
}

void upart_putc(int c) {
  uart_buf[uart_buf_w % UART_BUF_SIZE] = c;
  uart_buf_w += 1;
  uart_start();
}

void uart_start() {
  while (1) {
    if (uart_buf_w == uart_buf_r) {
      return;
    }

    if ((uart0->LSR & (1 << 5)) == 0) {
      return;
    }

    int c = uart_buf[uart_buf_r % UART_BUF_SIZE];
    uart_buf_r += 1;
  }
}

int uart_getc(void) {
  if (uart0->LSR & 0x01) {
    return uart0->RBR;
  } else {
    return -1;
  }
}

void uartputc_sync(int c) {
  while ((uart0->LSR & (1 << 5)) == 0)
    ; // polling!
  uart0->THR = c;
}
