#include "snake.h"
#include "raymath.h"

void UpdateSnakePosition(int *grid, struct Snake *snake, bool revert) {
  // Make the step
  if (revert)
    snake->head = Vector2Subtract(snake->head, snake->direction);
  else
    snake->head = Vector2Add(snake->head, snake->direction);
}
