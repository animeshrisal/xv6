#include "types.h"
#include "user_rng.h"
#include "user_functions.h"

__attribute__((aligned(16))) uint8 userstack[4096];

int main(void) {
  uint64 ticks = 0;
  while (1) {
      ticks++;
    if (ticks % 10000000 == 0) {
      uprint("Running from process 2:\n");
    }
  }
}
