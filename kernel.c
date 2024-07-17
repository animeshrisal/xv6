#include "kernel.h"
#include "hardware.h"
#include "riscv.h"
#include "syscalls.h"
#include "types.h"

extern int main(void);
extern void ex(void);
extern void printstring(char *s);

__attribute__((aligned(16))) char stack0[4096];

volatile struct uart *uart0 = (volatile struct uart *)0x10000000;

/*
void *memset(void *dst, int value, int num) {
  char *cdst = (char *)dst;

  int i;

  for (i = 0; i < num; i++) {
    cdst[i] = value;
  }

  return dst;
}
*/

static void putachar(char c) {
  while ((uart0->LSR & (1 << 5)) == 0)
    ;
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

void exception(void) {
  uint64 nr;
  uint64 param;
  uint64 retval = 0;

  asm volatile("mv %0, a7" : "=r"(nr) : :);
  asm volatile("mv %0, a7" : "=r"(param) : :);

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

  // Here, we adjust return value - we want to return to the instruction _after_
  // the ecall! (at address mepc+4)
  uint64 pc = r_mepc();
  w_mepc(pc + 4);

  asm volatile("mv a0, %0" : : "r"(retval) :);
}
