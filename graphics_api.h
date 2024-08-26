#include "types.h"

struct Pixel {
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
};

void set_pixel(Pixel *fb, uint32 x, uint32 y, Pixel &color) {
  if (x < 640 && y < 480) {
    fb[y * 640 + x] = color;
  }
};

void fill_rect(Pixel *fb, uint32 x, uint32 y, uint32 width, uint32 height,
               Pixel &color) {
  for (uint32 row = y; row < (y + height); row++) {
    for (uint32 col = x; col < (x + width); col++) {
      set_pixel(fb, col, row, color);
    }
  }
}
