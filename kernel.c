#include "hardware.h"
#include "riscv.h"
#include "syscalls.h"
#include "types.h"

extern int main(void);
extern void ex(void);
extern void printstring(char *s);

__attribute__((aligned(16))) char stack0[4096];

void printhex(uint64);

volatile struct uart *uart0 = (volatile struct uart *)0x10000000;
volatile struct uart *fb = (volatile struct uart *)0x40000000;

static void draw_frame() {}

static void putachar(char c) {
  while ((uart0->LSR & (1 << 5)) == 0)
    ; // polling!
  uart0->THR = c;
}

static void printastring(char *s) {
  while (*s) {
    putachar(*s);
    s++;
  }
}

static char getachar(void) {
  char c;
  while ((uart0->LSR & (1 << 0)) == 0)
    ;
  c = uart0->RBR;
  return c;
}

// This is a useful helper function (after you implemented putachar and
// printstring)
void printhex(uint64 x) {
  int i;
  char s;

  printastring("0x");
  for (i = 60; i >= 0; i -= 4) {
    int d = ((x >> i) & 0x0f);
    if (d < 10)
      s = d + '0';
    else
      s = d - 10 + 'a';
    putachar(s);
  }
  printastring("\n");
}

// This is the C code part of the exception handler
// "exception" is called from the assembler function "ex" in ex.S with registers
// saved on the stack
void exception(void) {
  uint64 nr;
  uint64 param;
  uint64 retval = 0;

  asm volatile("mv %0, a7" : "=r"(nr) : :);
  asm volatile("mv %0, a0" : "=r"(param) : :);

  switch (nr) {
  case PRINTASTRING:
    printastring((char *)param);
    break;
  case PUTACHAR:
    putachar((char)param);
    break;
  case GETACHAR:
    retval = (uint64)getachar();
    break;

  default:
    printastring("*** INVALID SYSCALL NUMBER!!! ***\n");
    break;
  }

  uint64 pc = r_mepc();
  w_mepc(pc + 4);

  asm volatile("mv a0, %0" : : "r"(retval) :);
}
