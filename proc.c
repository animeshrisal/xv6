#include "proc.h"
#include "riscv.h"
#include "tprintf.h"

uint64 current_pid;
proc processes[MAX_PROCS];

__attribute__((aligned(4096))) uint64 page_table[NPROC][512 * 3];

uint64 initialize_page_table(int proc) {
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
            (0xA0000000ULL + proc * 0x200000ULL) >> PGSHIFT << PERMSHIFT;
        page_table[proc][i] |= 0x7f;
      } else
        page_table[proc][i] = 0;
    }
  }

  return (uint64)&page_table[proc][0];
}

void proc_init() {

  for (int i = 0; i < NPROC; i++) {
    processes[i].pc = 0;
    processes[i].sp = 0x1ffff8 - 256;
    processes[i].base_address = 0xA0000000ULL + 0x200000 * i;
    processes[i].page_table_base = initialize_page_table(i);
    processes[i].state = NONE;
  }

  w_satp(MAKE_SATP(processes[0].page_table_base));

  asm volatile("sfence.vma zero, zero");

  current_pid = 0;
  processes[0].state = RUNNING;
  w_mscratch(processes[0].base_address);
}

void proc_intr() {
  processes[current_pid].state = RUNNING;
  w_mscratch(processes[current_pid].base_address);
}
