#include "cpu.h"
#include "kerneldef.h"
#include "riscv.h"
#include "trap.h"

__attribute__((aligned(4096))) uint64 cpu1_page_table[1][512 * 3];
__attribute__((aligned(4096))) uint64 cpu2_page_table[2][512 * 3];

uint64 cpuid() {
  uint64 id = r_tp();
  return id;
}

uint64 initialize_page_table(struct cpu *cpu, int process_id) {

  uint64(*page_table)[512 * 3] =
      (cpu->cpu_id == 0) ? cpu1_page_table : cpu2_page_table;

  for (int i = 0; i < 512; i++) {
    if (i == 0) {
      page_table[process_id][i] =
          ((uint64)&page_table[process_id][512]) >> PGSHIFT << PERMSHIFT;

      page_table[process_id][i] |= (1 << PERM_V);
    } else {
      page_table[process_id][i] = 0;
    }
  }

  for (int i = 512; i < 1024; i++) {
    if (i == 512) {
      page_table[process_id][i] =
          (cpu->memory_area + 0x200000 * process_id) >> PGSHIFT << PERMSHIFT;
      page_table[process_id][i] |= 0x7f;
    } else
      page_table[process_id][i] = 0;
  }

  return (uint64)&page_table[process_id][0];
}

void proc_init() {

  struct cpu *cpu = get_cpu();

  for (int i = 0; i < cpu->max_processes; i++) {

    cpu->processes[i].id = i;
    cpu->processes[i].pc = 0;
    cpu->processes[i].sp = 0x1ffff8 - 256;
    cpu->processes[i].base_address = cpu->memory_area + 0x200000 * i;

    cpu->processes[i].page_table_base = initialize_page_table(cpu, i);
    cpu->processes[i].state = i == 0 ? RUNNING : READY;
  }
  w_satp(MAKE_SATP(cpu->processes[0].page_table_base));

  asm volatile("sfence.vma zero, zero");

  w_mscratch(cpu->processes[0].base_address);
}

void proc_intr() {
  struct cpu *cpu = get_cpu();

  if (cpu->cpu_id == 1) {
    cpu->current_process = cpu->current_process == 0 ? 1 : 0;
  }

  w_satp(MAKE_SATP(cpu->processes[cpu->current_process].page_table_base));
  asm volatile("sfence.vma zero, zero");
  w_mscratch(cpu->processes[cpu->current_process].base_address);
}
