#include "utils.h"

#ifndef SNAKE_H
#define SNAKE_H

void UpdateSnakePosition(
    int *grid, struct Snake *snake); // Move the snake head in the grid; Wrap if
                                     // it surpasses grid edges
#endif
