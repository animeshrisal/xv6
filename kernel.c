#include "kerneldef.h"
#include "riscv.h"
#include "tprintf.h"
#include "trap.h"
#include "types.h"

extern int main(void);
extern void ex(void);
extern uint64 current_pid;
extern proc cpu1_processes[1];
extern proc cpu2_processes[1];

uint64 virt2phys(uint64 addr, proc process) {
  return process.base_address + addr;
}

uint64 phys2virt(uint64 addr, proc process) {
  return addr - process.base_address;
}

uint64 kernel_trap(registers *regs) {
  uint64 mepc = r_mepc();
  uint64 mstatus = r_mstatus();
  uint64 mcause = r_mcause();
  uint64 mtval = r_mtval();
  uint64 hartid = cpuid();

  proc *processes = (hartid == 0) ? cpu1_processes : cpu2_processes;

  int num_processes = (hartid == 0) ? 1 : 2;

  processes[0].pc = mepc;
  processes[0].sp = virt2phys(processes[0].sp, processes[0]);
  processes[0].state = READY;

  int cause = dev_intr(regs);

  // putting this makes the display run fast. i have no idea why
  tprintf(".");
  if (cause == 0) {
  }

  // Caused by clock interrupt
  if (cause == 2) {
  }

  // Caused by an ecall
  if (cause == 3) {
    w_mepc(processes[0].pc + 4);
  } else {
    w_mepc(processes[0].pc);
  }

  regs = (registers *)virt2phys(processes[0].sp, processes[0]);
  regs->sp = (uint64)regs;

  return (uint64)regs;
}
