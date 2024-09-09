#include "proc.h"
#include "riscv.h"

uint64 ticks;
uint64 current_pid;
extern proc processes[MAX_PROCS];

__attribute__((aligned(4096))) uint64 pt[NPROC][512 * 3];

uint64 init_pt(int proc) {
  for (int i = 0; i < 512; i++) {
    if (i == 0) {
      pt[proc][i] = ((uint64)&pt[proc][512]) >> PGSHIFT << PERMSHIFT;
      pt[proc][i] |= (1 << PERM_V);
    } else {
      pt[proc][i] = 0;
    }

    for (int i = 512; i < 1024; i++) {
      if (i == 512) {
        pt[proc][i] =
            (0x80000000ULL + (proc + 1) * 0x200000ULL) >> PGSHIFT << PERMSHIFT;
        pt[proc][i] |= 0x7f;
      } else {
        pt[proc][i] = 0;
      }
    }
  }

  return (uint64)&pt[proc][0];
}

void proc_init() {

  for (int i = 0; i < NPROC; i++) {
    processes[i].pc = 0;
    processes[i].sp = 0x1ffff8 - 256;
    processes[i].base_address = 0x80200000ULL + 0x200000 * i;
    processes[i].state = NONE;
  }

  w_satp(MAKE_SATP((uint64)&pt[0][0]));
  asm volatile("sfence.vma zero, zero");
}

void proc_intr() {
  processes[current_pid].state = READY;

  while (1) {
    current_pid = (current_pid + 1) % MAX_PROCS;
    if (processes[current_pid].state == READY) {
      processes[current_pid].state = RUNNING;
      break;
    }
  }

  w_mscratch(processes[current_pid].base_address);

  processes[current_pid].state = RUNNING;
}
