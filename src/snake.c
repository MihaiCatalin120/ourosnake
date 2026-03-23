#include "snake.h"
#include "config.h"
#include "raylib.h"
#include "raymath.h"

void UpdateSnakePosition(int *grid, struct Snake *snake) {
  // Make the step
  snake->head = Vector2Add(snake->head, snake->direction);

  // Wrap head position if outside of grid
  if (snake->head.x >= (int)NO_COLUMNS)
    snake->head.x = 0;
  if (snake->head.x < 0)
    snake->head.x = (int)NO_COLUMNS - 1;
  if (snake->head.y >= (int)NO_ROWS)
    snake->head.y = 0;
  if (snake->head.y < 0)
    snake->head.y = (int)NO_ROWS - 1;
}
