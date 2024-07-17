#include "syscalls.h"
#include "types.h"

int __attribute__((section(".usertext"))) main(void);

uint64 syscall(uint64 nr, uint64 param);

uint64 syscall(uint64 nr, uint64 param) {

  uint64 retval;

  asm volatile("mv a7, %0" : : "r"(nr) :);
  asm volatile("mv a0, %0" : : "r"(param) :);

  asm volatile("ecall");

  asm volatile("mv %0, a0" : "=r"(retval) : :);
  return retval;
}

void printastring(char *s) { syscall(PRINTASTRING, (uint64)s); }

void putachar(char c) { syscall(PUTACHAR, (uint64)c); }

char getachar(void) { return syscall(GETACHAR, 0); }

int main(void) {
  char c = 0;
  printastring("Hallo Bamberg!\n");

  do {
    c = getachar();
    printastring("reee\n");
    if (c >= 'a' && c <= 'z')
      c = c & ~0x20;
    putachar(c);

  } while (c != 'X');

  printastring("This is the end!\n");
  return 0;
}
