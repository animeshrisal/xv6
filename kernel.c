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
extern uint64 current_pid;
extern proc processes[MAX_PROCS];

__attribute__((aligned(16))) uint8 stack0[4096];

uint64 virt2phys(uint64 addr) {
  return processes[current_pid].base_address + addr;
}

uint64 phys2virt(uint64 addr) {
  return addr - processes[current_pid].base_address;
}

uint64 kernel_trap(registers *regs) {
  uint64 mepc = r_mepc();
  uint64 mstatus = r_mstatus();
  uint64 mcause = r_mcause();
  uint64 mtval = r_mtval();
  int cause = dev_intr();

  processes[current_pid].pc = mepc;
  processes[current_pid].sp = phys2virt((uint64)regs);
  processes[current_pid].state = READY;
  tprinthex(mcause);
  if (cause == 0) {

    tprintf("Error!");
    /*
     * Cause exception to happen
     * */

    tprinthex(mcause);
    tprintf("\n");
    tprinthex(mtval);
    tprintf("\n");
    tprinthex(mepc);
    tprintf("\n");
  }

  // Caused by clock interrupt
  if (cause == 2) {
  }

  // Caused by an ecall
  if (cause == 3) {

    w_mepc(processes[current_pid].pc + 4);
  } else {
    w_mepc(processes[current_pid].pc);
  }

  w_mscratch(processes[current_pid].base_address);

  regs = (registers *)virt2phys(processes[current_pid].sp);

  regs->a0 = 0;
  regs->sp = (uint64)regs;

  return (uint64)regs;
}
