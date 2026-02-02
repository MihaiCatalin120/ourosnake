#ifndef UNITY_BUILD
#define UNITY_BUILD
#include "raylib.h"

struct Snake {
  Vector2 head;
  Vector2 direction;
  int length;
};

void UpdateSnakePosition(
    int *grid, struct Snake *snake); // Move the snake head in the grid; Wrap if
                                     // it surpasses grid edges

#endif
