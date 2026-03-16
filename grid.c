#include "grid.h"
#include "config.h"
#include "raymath.h"
#include <stdio.h>

void UpdateCellLives(int *grid, struct Snake snake) {
  for (size_t y = 0; y < NO_ROWS; y += 1) {
    for (size_t x = 0; x < NO_COLUMNS; x += 1) {
      const int gridPosTranslated = y * NO_COLUMNS + x;

      if ((int)snake.head.x == x && (int)snake.head.y == y) {
        if (grid[gridPosTranslated] == CELL_EMPTY)
          grid[gridPosTranslated] = snake.length;
      } else if (grid[gridPosTranslated] > 0) {
        grid[gridPosTranslated] -= 1;
      }
    }
  }
}

void ClearGrid(int *grid) {
  for (size_t y = 0; y < NO_ROWS; y += 1) {
    for (size_t x = 0; x < NO_COLUMNS; x += 1) {
      grid[y * NO_COLUMNS + x] = CELL_EMPTY;
    }
  }
}

// TODO: Use vector distance to ensure goal is not too near of the starting
// position
void GenerateGoal(int *grid) {
  Vector2 pos = {GetRandomValue(0, NO_COLUMNS - 1),
                 GetRandomValue(0, NO_ROWS - 1)};
  // Avoid spawning directly on an obstacle or snake
  while (!IsValidSpawnPoint(grid, pos, 3)) {
    pos.x = GetRandomValue(0, NO_COLUMNS - 1);
    pos.y = GetRandomValue(0, NO_ROWS - 1);
  }

  grid[(int)pos.y * NO_COLUMNS + (int)pos.x] = CELL_GOAL;
}

bool IsValidSpawnPoint(int *grid, Vector2 head, int checkRadius) {
  Vector2 radius = {checkRadius, checkRadius};
  Vector2 start = Vector2Subtract(head, radius);
  Vector2 end = Vector2Add(head, radius);

  for (size_t x = start.x; x < end.x; x++) {
    int currentX = x;
    if (x < 0)
      currentX = NO_ROWS + x;

    if (x >= NO_ROWS)
      currentX = x - NO_ROWS;

    for (size_t y = start.y; y < end.y; y++) {
      int currentY = y;
      if (y < 0)
        currentY = NO_ROWS + y;

      if (y > NO_COLUMNS - 1)
        currentY = y - NO_ROWS;

      if (grid[currentY * NO_COLUMNS + currentX] != CELL_EMPTY)
        return false;
    }
  }
  return true;
}
