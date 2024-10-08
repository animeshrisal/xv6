#include "kerneldef.h"
#include "riscv.h"
#include "types.h"

#define NCPU 2

struct cpu {
  proc *processes;
  int current_process;
  int max_processes;
  uint64 cpu_id;
  uint64 memory_area;
  uint64 ticks;
};

struct cpu *get_cpu();
void init_cpu(int max_processes, uint64 memory_area);
