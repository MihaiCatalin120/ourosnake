#include "obstacle.h"
#include "config.h"
#include "raylib.h"
#include "raymath.h"
#include "utils.h"
#include <stdio.h>

void MarkObstacle(int *grid, struct ObstacleGenerator generator) {
  Vector2 head = generator.startPosition;
  Vector2 endPosition = {0, 0};
  size_t index = 0;

  while (Vector2Equals(generator.moves[index], endPosition) == 0) {
    if (grid[(int)head.y * NO_COLUMNS + (int)head.x] != CELL_GOAL) {
      grid[(int)head.y * NO_COLUMNS + (int)head.x] = CELL_OBSTACLE;
    }
    head = Vector2Add(head, generator.moves[index]);
    if ((int)head.x < 0 || (int)head.x >= NO_COLUMNS || (int)head.y < 0 ||
        (int)head.y >= NO_ROWS)
      break;
    index++;
  }
}

void GenerateInitialObstacles(int *grid, int numberOfObstacles) {
  Vector2 endDirection = {0, 0};

  for (size_t i = 0; i < numberOfObstacles; i++) {
    struct ObstacleGenerator generator;

    generator.startPosition.x = (int)GetRandomValue(0, NO_COLUMNS - 1);
    generator.startPosition.y = (int)GetRandomValue(0, NO_ROWS - 1);

    size_t moveNumber;
    for (moveNumber = 0; moveNumber < GetRandomValue(3, 6); moveNumber++) {
      generator.moves[moveNumber] = directions[GetRandomValue(0, 3)];
    }
    generator.moves[moveNumber] = endDirection;

    MarkObstacle(grid, generator);
  }
}
