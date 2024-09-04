#define UART_BUF_SIZE 32

void uart_intr(void);
void uart_start();
int uart_getc(void);

void console_intr(int c);
void uart_init(void);
void uartputc_sync(int c);
