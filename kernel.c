#include "cpu.h"
#include "kerneldef.h"
#include "riscv.h"
#include "tprintf.h"
#include "trap.h"
#include "types.h"

extern int main(void);
extern void ex(void);

uint64 virt2phys(uint64 addr, proc *process) {
  return process->base_address + addr;
}

uint64 phys2virt(uint64 addr, proc *process) {
  return addr - process->base_address;
}

uint64 kernel_trap(registers *regs) {

  uint64 mepc = r_mepc();
  uint64 mstatus = r_mstatus();
  uint64 mcause = r_mcause();
  uint64 mtval = r_mtval();
  uint64 hartid = cpuid();

  struct cpu *cpu = get_cpu();
  proc *proc = &cpu->processes[cpu->cpu_id];

  proc->pc = mepc;
  proc->sp = phys2virt((uint64)regs, proc);

  int cause = dev_intr(regs);

  if (cause == 0) {
  }

  // Caused by clock interrupt
  if (cause == 2) {
  }

  // Caused by an ecall

  if (cause == 3) {
    proc->pc += 4;
    w_mepc(proc->pc);

  } else {
    w_mepc(proc->pc);
  }

  regs = (registers *)virt2phys(proc->sp, proc);

  regs->sp = (uint64)regs;

  return (uint64)regs;
}
