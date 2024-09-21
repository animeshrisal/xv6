
#include "gpu_driver.h"
#include "kerneldef.h"
#include "syscall.h"

void set_pixels(Pixel *fb, uint32 x, uint32 y) {
  Pixel black = {.R = 0, .G = 0, .B = 0, .A = 0};

  const uint32 FRAMEBUFFER_WIDTH = 640;
  fb[y * FRAMEBUFFER_WIDTH + x] = black;
}

void make_rects(Pixel *fb, uint32 x, uint32 y, uint32 width, uint32 height) {

  Pixel black = {.R = 0, .G = 0, .B = 0, .A = 0};
  for (uint32 row = y; row < (y + height); row++) {
    for (uint32 col = x; col < (x + width); col++) {
      set_pixels(fb, col, row);
    }
  }
}

void fill_rects(Pixel *framebuffer) {
  int color = 255;
  Pixel white = {.R = color, .G = color, .B = color, .A = color};

  for (int i = 0; i < 640 * 480; i++) {
    framebuffer[i] = white;
  }
}

void clear_frames(Pixel *fb) {
  Pixel white = {.R = 255, .G = 255, .B = 255, .A = 255};

  for (uint32 i = 0; i < 640 * 480; i++) {
    fb[i] = white;
  }
}

void gpu_command(uint64 syscall, registers *regs) {
  uint64 framebuffer = get_framebuffer();

  switch (syscall) {
  case SYS_fill_rects:
    break;
  case SYS_make_rect:
    make_rects((Pixel *)framebuffer, regs->a0, regs->a1, regs->a2, regs->a3);
    break;
  case SYS_clear_frame:
    clear_frames((Pixel *)framebuffer);
    break;
  case SYS_set_pixel:
    set_pixels((Pixel *)framebuffer, regs->a0, regs->a1);
    break;
  case SYS_flush:
    transfer();
    break;
  }
}
