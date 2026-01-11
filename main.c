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
#define DEBUG_MODE false
#define TIME_PER_TURN 0.5f

struct Snake {
  Vector2 head;
  Vector2 direction;
  int length;
};

void updateSnakePosition(int *grid, struct Snake *snake) {
  snake->head = Vector2Add(snake->head, snake->direction);
  if (snake->head.x >= WINDOW_WIDTH * 1.0f / GRID_CELL_SIZE)
    snake->head.x = 0;
  if (snake->head.x < 0)
    snake->head.x = WINDOW_WIDTH * 1.0f / GRID_CELL_SIZE - 1;
  if (snake->head.y >= WINDOW_HEIGHT * 1.0f / GRID_CELL_SIZE)
    snake->head.y = 0;
  if (snake->head.y < 0)
    snake->head.y = WINDOW_HEIGHT * 1.0f / GRID_CELL_SIZE - 1;

  if (DEBUG_MODE)
    printf("[DEBUG]: Head is now x:%f y:%f\n", snake->head.x, snake->head.y);
}

void checkSnakeCollision(int *grid, struct Snake *snake) {}

void updateCellLives(int *grid, struct Snake snake) {
  for (size_t y = 0; y < WINDOW_HEIGHT / GRID_CELL_SIZE; y += 1) {
    for (size_t x = 0; x < WINDOW_WIDTH / GRID_CELL_SIZE; x += 1) {
      const int gridPosTranslated = y * WINDOW_WIDTH + x;

      if ((int)snake.head.x == x && (int)snake.head.y == y) {
        if (grid[gridPosTranslated] == 0)
          grid[gridPosTranslated] = snake.length;
        else {
          printf("ouch");
          WindowShouldClose();
        }
      } else if (grid[gridPosTranslated] > 0) {
        grid[gridPosTranslated] -= 1;
      }
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

bool isValueInArray(int value, int array[], int length) {
  for (size_t i = 0; i < length; i++) {
    if (value == array[i])
      return true;
  }

  return false;
}

void checkForDirectionChange(struct Snake *snake) {
  int key = GetKeyPressed();

  if (key == KEY_NULL)
    return;

  // Up, Right, Down, Left
  Vector2 directions[4] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};
  int upCases[2] = {KEY_UP, KEY_W};
  int rightCases[2] = {KEY_RIGHT, KEY_D};
  int downCases[2] = {KEY_DOWN, KEY_S};
  int leftCases[2] = {KEY_LEFT, KEY_A};

  if (isValueInArray(key, upCases, sizeof(upCases) / sizeof(upCases[0]))) {
    snake->direction = directions[0];
    return;
  }

  if (isValueInArray(key, rightCases,
                     sizeof(rightCases) / sizeof(rightCases[0]))) {
    snake->direction = directions[1];
    return;
  }

  if (isValueInArray(key, downCases,
                     sizeof(downCases) / sizeof(downCases[0]))) {
    snake->direction = directions[2];
    return;
  }

  if (isValueInArray(key, leftCases,
                     sizeof(leftCases) / sizeof(leftCases[0]))) {
    snake->direction = directions[3];
    return;
  }
}

// TODO: Configure LSP to work on current folder structure without any hacks
// (currently duplicating header files into source code directory just for LSP)
int main() {
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello");
  SetTargetFPS(60);
  int *grid = (int *)MemAlloc(WINDOW_WIDTH * WINDOW_HEIGHT / GRID_CELL_SIZE *
                              sizeof(int));

  // Initialize grid
  for (size_t y = 0; y < WINDOW_HEIGHT / GRID_CELL_SIZE; y += 1) {
    for (size_t x = 0; x < WINDOW_WIDTH / GRID_CELL_SIZE; x += 1) {
      grid[y * WINDOW_WIDTH + x] = 0;
    }
  }

  Vector2 head = {1, 1};
  Vector2 initialDirection = {1, 0};
  struct Snake snake = {head, initialDirection, 5};
  float time = 0;
  bool gameOver = false;

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();
    time += dt;

    checkForDirectionChange(&snake);
    if (time > TIME_PER_TURN && !gameOver) {
      time -= TIME_PER_TURN;
      updateSnakePosition(grid, &snake);
      if (grid[(int)snake.head.y * WINDOW_WIDTH + (int)snake.head.x] > 0) {
        gameOver = true;
        goto draw;
      }
      updateCellLives(grid, snake);
    }
    // if (DEBUG_MODE) {
    //   printf("[DEBUG]: Delta - %f\n", dt);
    //   printf("[DEBUG]: Time - %f\n", time);
    // }
  draw:
    BeginDrawing();
    ClearBackground(GetColor(0x202020FF));
    DrawObjects(grid);
    DrawFPS(700, 100);
    if (DEBUG_MODE) {
      DrawDebugCellValues(grid);
    }
    DrawGrid2D();
    if (gameOver) {
      DrawText("Game Over", WINDOW_WIDTH / 4, WINDOW_HEIGHT / 4, 64, RED);
    }
    EndDrawing();
  }

  CloseWindow();
  MemFree(grid);
  return 0;
}
