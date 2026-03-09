#include "grid.h"
#include "config.h"
#include "obstacle.h"
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

void ShiftGridIfNeeded(int *grid, struct Snake *snake,
                       double *averageTimePerCell) {
  int directionDelta = 0;
  int rowStart = 0;
  int columnStart = 0;
  int rowEnd = NO_ROWS;
  int columnEnd = NO_COLUMNS;
  // Right shift - Left column needs fill
  if (snake->head.x < GRID_SHIFT_THRESHOLD) {
    columnEnd = 1;
    directionDelta = 1;
  }
  // Left shift - Right column needs fill
  if (snake->head.x > NO_COLUMNS - GRID_SHIFT_THRESHOLD - 1) {
    columnStart = NO_COLUMNS - 1;
    directionDelta = -1;
  }
  // Down shift - Top column needs fill
  if (snake->head.y < GRID_SHIFT_THRESHOLD) {
    rowEnd = 1;
    directionDelta = NO_COLUMNS;
  }
  // Up shift - Bottom column needs fill
  if (snake->head.y > NO_ROWS - GRID_SHIFT_THRESHOLD - 1) {
    rowStart = NO_ROWS - 1;
    directionDelta = -NO_COLUMNS;
  }

  switch (directionDelta) {
  case -1:
  case -NO_COLUMNS:
    for (size_t y = 0; y < NO_ROWS; y += 1) {
      for (size_t x = 0; x < NO_COLUMNS; x += 1) {
        const int gridPosTranslated = y * NO_COLUMNS + x;
        const int targetPos = gridPosTranslated + directionDelta;
        if (targetPos < 0 || targetPos >= NO_COLUMNS * NO_ROWS)
          continue;

        grid[targetPos] = grid[gridPosTranslated];
      }
    }
    break;
  case 1:
    for (size_t y = 0; y < NO_ROWS; y += 1) {
      for (size_t x = NO_COLUMNS - 1; x > 0; x -= 1) {
        const int gridPosTranslated = y * NO_COLUMNS + x;
        const int targetPos = gridPosTranslated - directionDelta;
        if (targetPos < 0 || targetPos >= NO_COLUMNS * NO_ROWS)
          continue;

        grid[gridPosTranslated] = grid[targetPos];
      }
    }
    break;
  case NO_COLUMNS:
    for (size_t y = NO_ROWS - 1; y > 0; y -= 1) {
      for (size_t x = 0; x < NO_COLUMNS; x += 1) {
        const int gridPosTranslated = y * NO_COLUMNS + x;
        const int targetPos = gridPosTranslated - directionDelta;
        if (targetPos < 0 || targetPos >= NO_COLUMNS * NO_ROWS)
          continue;

        grid[gridPosTranslated] = grid[targetPos];
      }
    }
    break;
  default:
    return;
  }

  GenerateEdgeObstacles(grid, rowStart, columnStart, rowEnd, columnEnd,
                        directionDelta, averageTimePerCell);

  UpdateSnakePosition(grid, snake, true);
}
