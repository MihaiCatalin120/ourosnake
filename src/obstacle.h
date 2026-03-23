#include "utils.h"

#ifndef OBSTACLE_H
#define OBSTACLE_H

void MarkObstacle(int *grid,
                  struct ObstacleGenerator
                      generator); // Mark the grid cells as occupied by the
                                  // positions defined in the obstacle structure
void GenerateInitialObstacles(
    int *grid,
    int numberOfObstacles); // Put n obstacle structures on the given grid
                            //
#endif
