#include "grid.h"
#include "config.h"
#include "raylib.h"
#include "raymath.h"
#include <stdio.h>

void UpdateCellLives(int *grid, struct Snake snake) {
  for (size_t y = 0; y < NO_ROWS; y += 1) {
    for (size_t x = 0; x < NO_COLUMNS; x += 1) {
      const int gridPosTranslated = y * NO_COLUMNS + x;

      if ((int)snake.head.x == x && (int)snake.head.y == y) {
        if (grid[gridPosTranslated] == CELL_EMPTY ||
            grid[gridPosTranslated] == CELL_POWERUP_LENGTH_INCREASE)
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
  Vector2 pos =
      GetValidGridPosition(grid, CHECK_RADIUS, MAX_TRIES_PER_CHECK_RADIUS);
  if (pos.x == -1 || pos.y == -1)
    return;

  grid[(int)pos.y * NO_COLUMNS + (int)pos.x] = CELL_GOAL;
}

void GeneratePowerupLengthIncrease(int *grid) {
  Vector2 pos =
      GetValidGridPosition(grid, CHECK_RADIUS, MAX_TRIES_PER_CHECK_RADIUS);

  if (pos.x == -1 || pos.y == -1)
    return;

  grid[(int)pos.y * NO_COLUMNS + (int)pos.x] = CELL_POWERUP_LENGTH_INCREASE;
}

bool IsValidSpawnPoint(int *grid, Vector2 head, int checkRadius) {
  Vector2 radius = {checkRadius, checkRadius};
  Vector2 start = Vector2Subtract(head, radius);
  Vector2 end = Vector2Add(head, radius);

  for (int x = start.x; x < end.x; x++) {
    int currentX = x;
    if (x < 0)
      currentX = NO_COLUMNS + x;

    if (x >= NO_COLUMNS)
      currentX = x - NO_COLUMNS;

    for (int y = start.y; y < end.y; y++) {
      int currentY = y;
      if (y < 0)
        currentY = NO_ROWS + y;

      if (y > NO_ROWS - 1)
        currentY = y - NO_ROWS;

      if (grid[currentY * NO_COLUMNS + currentX] != CELL_EMPTY)
        return false;
    }
  }
  return true;
}

void IncreaseSnakeLength(int *grid, struct Snake *snake) {
  snake->length++;
  for (size_t y = 0; y < NO_ROWS; y += 1) {
    for (size_t x = 0; x < NO_COLUMNS; x += 1) {
      if (grid[y * NO_COLUMNS + x] > 0)
        grid[y * NO_COLUMNS + x]++;
      ;
    }
  }
}

Vector2 GetValidGridPosition(int *grid, int checkRadius,
                             int maxRetriesPerRadius) {
  Vector2 result = {GetRandomValue(0, NO_COLUMNS - 1),
                    GetRandomValue(0, NO_ROWS - 1)};
  if (IsValidSpawnPoint(grid, result, checkRadius))
    return result;

  while (checkRadius > 0) {
    for (size_t i = 0; i < maxRetriesPerRadius; i++) {
      result.x = GetRandomValue(0, NO_COLUMNS - 1);
      result.y = GetRandomValue(0, NO_ROWS - 1);

      if (IsValidSpawnPoint(grid, result, checkRadius))
        return result;
    }

    checkRadius--;
  }

  Vector2 fail = {-1, -1};
  return fail;
}
