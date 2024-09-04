#include "display.h"
#include "hardware.h"
#include "riscv.h"
#include "syscall.h"
#include "trap.h"
#include "types.h"

extern int main(void);
extern void ex(void);
extern void printstring(char *s);

__attribute__((aligned(16))) char stack0[4096];

void kernel_trap() {
  uint64 mepc = r_mepc();
  uint64 mstatus = r_mstatus();
  uint64 mcause = r_mcause();

  int cause = dev_intr();

  if (cause == 0) {
    /*
     * Cause exception to happen
     * */
  }

  w_mepc(mepc);
  w_mstatus(mstatus);
}
