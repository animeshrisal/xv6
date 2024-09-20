#include "types.h"

#define MAX_PROCS 3
#define SATP_SV39 (8L << 60)
#define MAKE_SATP(pagetable) (SATP_SV39 | (((uint64)pagetable) >> 12))

#define NPROC 8
#define PGSHIFT 12
#define PERMSHIFT 10
#define PERM_V 0
#define PERM_X 3
#define PERM_W 2
#define PERM_R 1
#define PERM_U 4

typedef struct {
  uint64 ra;
  uint64 sp;
  uint64 gp;
  uint64 tp;
  uint64 t0;
  uint64 t1;
  uint64 t2;
  uint64 s0;
  uint64 s1;
  uint64 a0;
  uint64 a1;
  uint64 a2;
  uint64 a3;
  uint64 a4;
  uint64 a5;
  uint64 a6;
  uint64 a7;
  uint64 s2;
  uint64 s3;
  uint64 s4;
  uint64 s5;
  uint64 s6;
  uint64 s7;
  uint64 s8;
  uint64 s9;
  uint64 s10;
  uint64 s11;
  uint64 t3;
  uint64 t4;
  uint64 t5;
  uint64 t6;
} registers;

typedef enum { NONE, READY, RUNNING, BLOCKED, SLEEPING } proc_state;

typedef struct {
  uint64 pc;
  uint64 sp;
  uint64 base_address;
  uint64 page_table_base;
  uint64 id;
  proc_state state;
} proc;

void proc_init();
void proc_intr();
