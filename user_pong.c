#include "user_pong.h"
#include "display.h"
#include "tprintf.h"
#include "types.h"

#define NUM_CUBES 40
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
// Structure for a cube
typedef struct {
  int x, y;          // Position
  int width, height; // Size
  int vx, vy;        // Velocity
  Pixel color;       // Color
} Cube;

Cube cubes[NUM_CUBES];

// Function to set a pixel in the framebuffer
void set_pixel(Pixel *fb, uint32 x, uint32 y, Pixel color) {
  // Assuming the framebuffer is a linear array with a known width,
  // for example, a 640x480 framebuffer
  const uint32 FRAMEBUFFER_WIDTH =
      640; // Update according to your actual framebuffer width
  fb[y * FRAMEBUFFER_WIDTH + x] = color;
}

// Function to fill a rectangular area in the framebuffer
void make_rect(Pixel *fb, uint32 x, uint32 y, uint32 width, uint32 height,
               Pixel color) {
  for (uint32 row = y; row < (y + height); row++) {
    for (uint32 col = x; col < (x + width); col++) {
      set_pixel(fb, col, row, color);
    }
  }
}

static struct ball {
  int x;
  int y;
} ball;

void fill_rects(Pixel *framebuffer) {
  int color = 255;
  Pixel white = {.R = color, .G = color, .B = color, .A = color};

  for (int i = 0; i < 640 * 480; i++) {
    framebuffer[i] = white;
  }
}

void clear_frame(Pixel *fb, Pixel color) {
  for (uint32 i = 0; i < 640 * 480; i++) {
    fb[i] = color;
  }
}

// Function to detect collision between two cubes
int check_collision(Cube *a, Cube *b) {
  return (a->x < b->x + b->width && a->x + a->width > b->x &&
          a->y < b->y + b->height && a->y + a->height > b->y);
}

// Update the positions of the cubes and handle collisions
void update_cubes() {
  for (int i = 0; i < NUM_CUBES; i++) {
    // Update position
    cubes[i].x += cubes[i].vx;
    cubes[i].y += cubes[i].vy;

    // Bounce off the walls
    if (cubes[i].x <= 0 || cubes[i].x + cubes[i].width >= SCREEN_WIDTH) {
      cubes[i].vx = -cubes[i].vx;
    }
    if (cubes[i].y <= 0 || cubes[i].y + cubes[i].height >= SCREEN_HEIGHT) {
      cubes[i].vy = -cubes[i].vy;
    }

    // Check for collisions with other cubes
    for (int j = i + 1; j < NUM_CUBES; j++) {
      if (check_collision(&cubes[i], &cubes[j])) {
        // Simple collision response: reverse velocities
        cubes[i].vx = -cubes[i].vx;
        cubes[i].vy = -cubes[i].vy;
        cubes[j].vx = -cubes[j].vx;
        cubes[j].vy = -cubes[j].vy;
      }
    }
  }
}

// Render the cubes to the framebuffer
void render_cubes(Pixel *framebuffer) {
  for (int i = 0; i < NUM_CUBES; i++) {
    make_rect(framebuffer, cubes[i].x, cubes[i].y, cubes[i].width,
              cubes[i].height, cubes[i].color);
  }
}

void initialize_cubes() {
  for (int i = 0; i < NUM_CUBES; i++) {
    cubes[i].x = (i * 50) % 640;
    cubes[i].y = (i * 30) % 480;
    cubes[i].width = 32;
    cubes[i].height = 32;
    cubes[i].vx = (i % 2 == 0) ? 1 : -1;
    cubes[i].vy = (i % 2 == 0) ? 1 : -1;
    cubes[i].color = (Pixel){.R = 50 * i,
                             .G = 255 - (20 * i),
                             .B = 50,
                             .A = 255}; // Different colors
  }
}

int user_init() {
  Pixel *framebuffer;
  Pixel black = {.R = 0, .G = 0, .B = 0, .A = 255};
  ball.x = 0;
  ball.y = 0;
  gpuinit();
  asm volatile("mv %0, a1" : "=r"(framebuffer) : :);
  int i = 0;
  int j = 0;

  initialize_cubes();
  while (1) {
    // Clear the screen
    clear_frame(framebuffer, black);

    // Update the positions and handle collisions
    update_cubes();

    // Render the cubes
    render_cubes(framebuffer);

    flush();
  }
  return 0;
}
