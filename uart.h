#ifndef UART_H
#define UART_H
#include "types.h"

void uart_intr(void);
void uart_start();
int uart_getc(void);
void uart_interrupt_enable();

void console_intr(int c);
void uart_init(void);
void uartputc_sync(int c);
void get_char();

#endif
