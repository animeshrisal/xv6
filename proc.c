#include "kerneldef.h"
#include "riscv.h"
#include "tprintf.h"

uint64 current_pid = 0;
proc cpu1_processes[1];
proc cpu2_processes[2];

__attribute__((aligned(4096))) uint64 cpu1_page_table[1][512 * 3];
__attribute__((aligned(4096))) uint64 cpu2_page_table[2][512 * 3];

int cpuid() {
  int id = r_tp();
  return id;
}
uint64 initialize_page_table(int hartid, int proc) {
  uint64(*page_table)[512 * 3] =
      (hartid == 0) ? cpu1_page_table : cpu2_page_table;

  for (int i = 0; i < 512; i++) {
    if (i == 0) {
      page_table[proc][i] =
          ((uint64)&page_table[proc][512]) >> PGSHIFT << PERMSHIFT;

      page_table[proc][i] |= (1 << PERM_V);
    } else {
      page_table[proc][i] = 0;
    }

    for (int i = 512; i < 1024; i++) {
      if (i == 512) {
        page_table[proc][i] =
            (0x80000000ULL + (proc + 1) * 0x200000ULL) >> PGSHIFT << PERMSHIFT;
        page_table[proc][i] |= 0x7f;
      } else
        page_table[proc][i] = 0;
    }
  }

  return (uint64)&page_table[proc][0];
}

void proc_init() {
  int hartid = cpuid();
  proc *processes = (hartid == 0) ? cpu1_processes : cpu2_processes;

  int num_processes = (hartid == 0) ? 1 : 2;
  int addr = 0x80000000ULL + hartid * 0x01000000;

  for (int i = 0; i < num_processes; i++) {
    processes[i].pc = 0;
    processes[i].sp = 0x1ffff8 - 256;
    processes[i].base_address = addr + 0x200000 * i;
    processes[i].page_table_base = initialize_page_table(hartid, i);
    processes[i].state = NONE;
  }

  w_satp(MAKE_SATP(processes[0].page_table_base));

  asm volatile("sfence.vma zero, zero");
  current_pid = 0;
  processes[0].state = RUNNING;
  processes[1].state = READY;
  processes[2].state = READY;
  w_mscratch(processes[0].base_address);
}

void proc_intr() {
  int hartid = cpuid();
  proc *processes = (hartid == 0) ? cpu1_processes : cpu2_processes;

  while (1) {
    current_pid = (current_pid + 1) % MAX_PROCS;

    if (processes[current_pid].state == READY) {
      processes[current_pid].state = RUNNING;
      break;
    }
  }

  w_satp(MAKE_SATP(processes[current_pid].page_table_base));
  asm volatile("sfence.vma zero, zero");
  w_mscratch(processes[current_pid].base_address);
}
