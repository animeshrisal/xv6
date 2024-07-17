#include "display.h"
#include "types.h"

static uint64 *framebuffer;

void fb_init(void) {

  framebuffer = (uint64 *)FB_BASE;
  memset(framebuffer, 0, FB_WIDTH * FB_WIDTH * (FB_DEPTH / 8));
};

void fb_draw_pixel(int x, int y, uint32 color) {
  if (x < 0 || x >= FB_WIDTH || y < 0 || y >= FB_HEIGHT) {
    return;
  }
  framebuffer[y * FB_WIDTH + x] = color;
}

void fb_test(void) {
  fb_init();
  for (int y = 0; y < FB_HEIGHT; y++) {
    for (int x = 0; x < FB_WIDTH; x++) {
      fb_draw_pixel(x, y, 0x00FF00FF); // Example: purple color
    }
  }
}
