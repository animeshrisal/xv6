#include "user_collision.h"
#include "display.h"
#include "types.h"

__attribute__((aligned(16))) uint8 userstack[4096];

#define NUM_CUBES 20
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

static struct ball {
  int x;
  int y;
} ball;

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
void render_cubes() {
  for (int i = 0; i < NUM_CUBES; i++) {
    make_rect(cubes[i].x, cubes[i].y, cubes[i].width, cubes[i].height);
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

int main(void) {

  ball.x = 0;
  ball.y = 0;

  int i = 0;
  int j = 0;

  initialize_cubes();
  while (1) {
    clear_frame();
    update_cubes();
    render_cubes();
    flush();
  }

  return 0;
}
