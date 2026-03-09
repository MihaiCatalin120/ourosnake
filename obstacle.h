#include "utils.h"
#include <stddef.h>

#ifndef OBSTACLE_H
#define OBSTACLE_H

struct MarkEdgeThreadArgs {
  int id;
  int *grid;
  size_t x;
  size_t y;
  int directionDelta;
  int complementaryDirectionDelta;
  float *spawnThresholds;
};

void MarkObstacle(int *grid,
                  struct ObstacleGenerator
                      generator); // Mark the grid cells as occupied by the
                                  // positions defined in the obstacle structure
void GenerateInitialObstacles(
    int *grid,
    int numberOfObstacles); // Put n obstacle structures on the given grid
                            //
                            //
void MarkEdgeObstacle(int *grid, int x, int y, int directionDelta,
                      int complementaryDirectionDelta, float *spawnThresholds);

void *MarkEdgeObstacleThreaded(void *threadArgs);

void GenerateEdgeObstacles(int *grid, int rowStart, int columnStart, int rowEnd,
                           int columnEnd, int directionDelta,
                           double *averageTimePerCell);
#endif
