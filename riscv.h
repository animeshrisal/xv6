
#include "types.h"

#define MSTATUS_MPP_MASK (3L << 11)
#define MSTATUS_MPP_M (3l << 11)
#define MSTATUS_MPP_S (1L << 11)
#define MSTATUS_MPP_U (0L << 11)
#define MSTATUS_MIE (1L << 3)

static inline uint64 r_mcause() {
  uint64 x;
  asm volatile("csrr %0, mcause" : "=r"(x));
  return x;
}

static inline uint64 r_mstatus() {
  uint64 x;
  asm volatile("csrr %0, mstatus" : "=r"(x));
  return x;
}

static inline void w_mstatus(uint64 x) {
  asm volatile("csrw mstatus, %0" : : "r"(x));
};

static inline void w_mepc(uint64 x) {
  asm volatile("csrw mepc, %0" : : "r"(x));
}
