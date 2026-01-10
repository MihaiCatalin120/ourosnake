#include <raylib.h>
#include <raymath.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define GRID_CELL_SIZE 100

void DrawGrid2D(Vector2 *head, int *grid) {
  for (size_t y = 0; y < WINDOW_HEIGHT / GRID_CELL_SIZE; y += 1) {
    for (size_t x = 0; x < WINDOW_WIDTH / GRID_CELL_SIZE; x += 1) {
      Color currentPosColor = GetColor(0x3333FFFF);
      const int gridPosTranslated = y * WINDOW_WIDTH + x;
      // printf("[DEBUG]: x-y %lu-%lu\n", x, y);
      // printf("[DEBUG]: WINDOW_WIDTH - WINDOW_HEIGHT %d-%d\n", WINDOW_WIDTH,
      //        WINDOW_HEIGHT);
      printf("[DEBUG]: grid[%lu][%lu]: %d\n", x, y, grid[gridPosTranslated]);

      if ((int)head->x == x && (int)head->y == y) {
        if (grid[gridPosTranslated] == 0)
          grid[gridPosTranslated] = 8;
        else
          WindowShouldClose();
      }
      if (grid[gridPosTranslated] > 0) {
        currentPosColor = GetColor(0x33FF3380);
        grid[gridPosTranslated] -= 1;
      }
      DrawRectangle(x * GRID_CELL_SIZE + 1, y * GRID_CELL_SIZE + 1,
                    GRID_CELL_SIZE - 1, GRID_CELL_SIZE - 1, currentPosColor);

      // Debug text to show grid values, comment if not used
      char text[2];
      sprintf(text, "%d", grid[gridPosTranslated]);
      DrawText(text, x * GRID_CELL_SIZE + GRID_CELL_SIZE / 4,
               y * GRID_CELL_SIZE + GRID_CELL_SIZE / 4,
               Clamp(GRID_CELL_SIZE, 10, 64), GetColor(0x000000FF));
      // End debug

      head->x += 1;
      if (head->x > WINDOW_WIDTH * 1.0f / GRID_CELL_SIZE)
        head->x = 0;
      if (head->y > WINDOW_HEIGHT * 1.0f / GRID_CELL_SIZE)
        head->y = 0;

      printf("[DEBUG]: Head is now x:%f y:%f\n", head->x, head->y);
    }
  }
}

int main() {
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello");
  SetTargetFPS(60);
  int *grid =
      calloc(WINDOW_WIDTH * WINDOW_HEIGHT / GRID_CELL_SIZE, sizeof(int));
  Vector2 head = {1, 1};

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();
    BeginDrawing();
    DrawFPS(700, 100);
    ClearBackground(GetColor(0x202020FF));
    DrawGrid2D(&head, grid);
    EndDrawing();
  }

  CloseWindow();
  free(grid);
  return 0;
}
