#include "display.h"
#include "hardware.h"
#include "proc.h"
#include "riscv.h"
#include "syscall.h"
#include "tprintf.h"
#include "trap.h"
#include "types.h"

extern int main(void);
extern void ex(void);
extern uint64 pt[8][512 * 3];

__attribute__((aligned(16))) uint8 stack0[4096];

void kernel_trap(registers *regs) {
  uint64 mepc = r_mepc();
  uint64 mstatus = r_mstatus();
  uint64 mcause = r_mcause();

  int cause = dev_intr();

  if (cause == 0) {
    /*
     * Cause exception to happen
     * */

    tprintf("Error!\n");
  }

  // Caused by clock interrupt
  if (cause == 2) {
  }

  // Caused by an ecall
  if (cause == 3) {
    mepc += 4;
  }

  w_mepc(mepc);
  w_mstatus(mstatus);
}
