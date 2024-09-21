#include "cpu.h"
#include "kerneldef.h"

struct cpu cpus[2];
proc cpu1_processes[1];
proc cpu2_processes[2];

void init_cpu(int max_processes, uint64 memory_area) {
  uint64 hart_id = cpuid();

  struct cpu *cpu = &cpus[hart_id];

  cpu->processes = hart_id == 0 ? cpu1_processes : cpu2_processes;
  cpu->cpu_id = hart_id;
  cpu->current_process = 0;
  cpu->max_processes = max_processes;
  cpu->memory_area = memory_area;
  cpu->ticks = 0;
};

struct cpu *get_cpu() {
  uint64 hartid = cpuid();
  return &cpus[hartid];
}
