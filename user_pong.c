#include "user_pong.h"
#include "display.h"

Pixel *fb;

// Function to set a pixel in the framebuffer
void set_pixel(Pixel *fb, uint32 x, uint32 y, Pixel color) {
  // Assuming the framebuffer is a linear array with a known width,
  // for example, a 640x480 framebuffer
  const uint32 FRAMEBUFFER_WIDTH =
      640; // Update according to your actual framebuffer width
  fb[y * FRAMEBUFFER_WIDTH + x] = color;
}

// Function to fill a rectangular area in the framebuffer
void fill_rect(Pixel *fb, uint32 x, uint32 y, uint32 width, uint32 height,
               Pixel color) {
  for (uint32 row = y; row < (y + height); row++) {
    for (uint32 col = x; col < (x + width); col++) {
      set_pixel(fb, col, row, color);
    }
  }
}

void fill_rects(int color) {

  for (int i = 0; i < 640 * 480; i++) {
    Pixel white = {.R = (color + i) % 255,
                   .G = (254 - color + i) % 255,
                   .B = color,
                   .A = color};
    gpu.framebuffer[i] = white;
  }
}

void clear_frame() {}

struct pad {
  int x;
  int y;
};

struct ball {
  int x;
  int y;
};

void update() {}

int user_pong_process() {
  while (1) {
  }

  return 0;
}
