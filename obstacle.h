#ifndef UNITY_BUILD
#define UNITY_BUILD
#include "raylib.h"

struct ObstacleGenerator {
  Vector2 startPosition;
  Vector2 moves[10]; // TODO: support dynamic moves max size
};

void MarkObstacle(int *grid,
                  struct ObstacleGenerator
                      generator); // Mark the grid cells as occupied by the
                                  // positions defined in the obstacle structure
void GenerateInitialObstacles(
    int *grid,
    int numberOfObstacles); // Put n obstacle structures on the given grid
#endif
