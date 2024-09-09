#include "types.h"

#define MAX_PROCS 8
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

typedef enum { NONE, READY, RUNNING, BLOCKED, SLEEPING } proc_state;

typedef struct {
  uint64 pc;
  uint64 sp;
  uint64 base_address;
  uint64 page_table_base;
  proc_state state;
} proc;
