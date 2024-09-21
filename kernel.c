#include "cpu.h"
#include "riscv.h"
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

  struct cpu *cpu = get_cpu();

  cpu->processes[cpu->current_process].pc = mepc;
  cpu->processes[cpu->current_process].sp =
      phys2virt((uint64)regs, &cpu->processes[cpu->current_process]);
  cpu->processes[cpu->current_process].state = READY;

  int cause = dev_intr(regs);

  if (cause == 0) {

  }

  // Caused by clock interrupt
  if (cause == 2) {
  }

  // Get new process
  if (cause == 3) {
        if(cpu->cpu_id == 0) {
    tprinthex(cpu->processes[cpu->current_process].pc);
    }

    cpu->processes[cpu->current_process].pc += 4;
    w_mepc(cpu->processes[cpu->current_process].pc);

  } else {
    w_mepc(cpu->processes[cpu->current_process].pc);
  }


  regs = (registers *)virt2phys(cpu->processes[cpu->current_process].sp,
                                &cpu->processes[cpu->current_process]);

  regs->sp = (uint64)regs;

  return (uint64)regs;
}
