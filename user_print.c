#include "user_print.h"
#include "types.h"
#include "user_functions.h"

__attribute__((aligned(16))) uint8 userstack[4096];

int main(void) {
  uint64 ticks = 0;

  while (1) {
    ticks++;
    if (ticks % 1000000 == 0) {
      uprint("Running from process 1\n");
    }
  }
}
