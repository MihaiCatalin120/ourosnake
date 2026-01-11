#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define GRID_CELL_SIZE 100
#define DEBUG_MODE true

void updateSnakePosition(int *grid, Vector2 *head) {
  printf("update snake called \n");

  head->x += 1;
  printf("head increased\n");
  if (head->x > WINDOW_WIDTH * 1.0f / GRID_CELL_SIZE)
    head->x = 0;
  if (head->y > WINDOW_HEIGHT * 1.0f / GRID_CELL_SIZE)
    head->y = 0;

  if (DEBUG_MODE)
    printf("[DEBUG]: Head is now x:%f y:%f\n", head->x, head->y);
}

void updateCellLives(int *grid, Vector2 head) {
  for (size_t y = 0; y < WINDOW_HEIGHT / GRID_CELL_SIZE; y += 1) {
    for (size_t x = 0; x < WINDOW_WIDTH / GRID_CELL_SIZE; x += 1) {
      const int gridPosTranslated = y * WINDOW_WIDTH + x;

      if ((int)head.x == x && (int)head.y == y) {
        if (grid[gridPosTranslated] == 0)
          grid[gridPosTranslated] = 8;
        // else
        //   WindowShouldClose();
      }
      if (grid[gridPosTranslated] > 0) {
        grid[gridPosTranslated] -= 1;
      }

      // DrawRectangle(x * GRID_CELL_SIZE + 1, y * GRID_CELL_SIZE + 1,
      //               GRID_CELL_SIZE - 1, GRID_CELL_SIZE - 1, currentPosColor);
    }
  }
}

void DrawGrid2D() {
  for (size_t x = 0; x < WINDOW_WIDTH / GRID_CELL_SIZE; x += 1) {
    DrawLine(x * GRID_CELL_SIZE, 0, x * GRID_CELL_SIZE, WINDOW_HEIGHT, WHITE);
  }

  for (size_t y = 0; y < WINDOW_HEIGHT / GRID_CELL_SIZE; y += 1) {
    DrawLine(0, y * GRID_CELL_SIZE, WINDOW_WIDTH, y * GRID_CELL_SIZE, WHITE);
  }
}

void DrawDebugCellValues(int *grid) {
  for (size_t y = 0; y < WINDOW_HEIGHT / GRID_CELL_SIZE; y += 1) {
    for (size_t x = 0; x < WINDOW_WIDTH / GRID_CELL_SIZE; x += 1) {
      const int gridPosTranslated = y * WINDOW_WIDTH + x;

      if (DEBUG_MODE) {
        // Debug text to show grid values, comment if not used
        char text[2];
        sprintf(text, "%d", grid[gridPosTranslated]);
        DrawText(text, x * GRID_CELL_SIZE + GRID_CELL_SIZE / 4,
                 y * GRID_CELL_SIZE + GRID_CELL_SIZE / 4,
                 Clamp(GRID_CELL_SIZE, 10, 64), GetColor(0x000000FF));
        // End debug
      }
    }
  }
}

void DrawObjects(int *grid) {
  for (size_t y = 0; y < WINDOW_HEIGHT / GRID_CELL_SIZE; y += 1) {
    for (size_t x = 0; x < WINDOW_WIDTH / GRID_CELL_SIZE; x += 1) {
      const int gridPosTranslated = y * WINDOW_WIDTH + x;

      if (grid[gridPosTranslated] > 0) {
        DrawRectangle(x * GRID_CELL_SIZE + 1, y * GRID_CELL_SIZE + 1,
                      GRID_CELL_SIZE - 1, GRID_CELL_SIZE - 1,
                      GetColor(0x00FFFFFF));
      }
    }
  }
}

// TODO: Configure LSP to work on current folder structure without any hacks
// (currently duplicating header files into source code directory just for LSP)
int main() {
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello");
  SetTargetFPS(60);
  int *grid = (int *)MemAlloc(WINDOW_WIDTH * WINDOW_HEIGHT / GRID_CELL_SIZE *
                              sizeof(int));

  for (size_t y = 0; y < WINDOW_HEIGHT / GRID_CELL_SIZE; y += 1) {
    for (size_t x = 0; x < WINDOW_WIDTH / GRID_CELL_SIZE; x += 1) {
      grid[y * WINDOW_WIDTH + x] = 0;
    }
  }
  Vector2 head = {1, 1};
  float time = 0;

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();
    time += dt;

    if (time > 1.0f) {
      time = 0.0f;
      updateSnakePosition(grid, &head);
      updateCellLives(grid, head);
    }
    // if (DEBUG_MODE) {
    //   printf("[DEBUG]: Delta - %f\n", dt);
    //   printf("[DEBUG]: Time - %f\n", time);
    // }
    BeginDrawing();
    ClearBackground(GetColor(0x202020FF));
    DrawObjects(grid);
    DrawFPS(700, 100);
    if (DEBUG_MODE) {
      DrawDebugCellValues(grid);
    }
    DrawGrid2D();
    EndDrawing();
  }

  CloseWindow();
  MemFree(grid);
  return 0;
}
