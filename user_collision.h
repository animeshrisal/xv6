#include "types.h"

int user_init();
int gpuinit();
int flush();
int uprintf(uint64 chars);
int uprinthex(uint64 chars);
int make_rect(uint64 x, uint64 y, uint64 width, uint64 height);

int clear_frame();
