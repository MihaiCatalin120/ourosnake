#include "grid.h"
#include "config.h"
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
  while (grid[(int)pos.y * NO_COLUMNS + (int)pos.x] != CELL_EMPTY) {
    pos.x = GetRandomValue(0, NO_COLUMNS - 1);
    pos.y = GetRandomValue(0, NO_ROWS - 1);
  }

  grid[(int)pos.y * NO_COLUMNS + (int)pos.x] = CELL_GOAL;
}
