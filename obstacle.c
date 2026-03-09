#include "obstacle.h"
#include "config.h"
#include "raylib.h"
#include "raymath.h"
#include "utils.h"
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

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

void MarkEdgeObstacle(int *grid, int x, int y, int directionDelta,
                      int complementaryDirectionDelta, float *spawnThresholds) {
  const int gridPosTranslated = y * NO_COLUMNS + x;
  int obstaclesNearbyNumber = 0;

  int targetNeighbourPositions[3] = {
      gridPosTranslated + directionDelta + complementaryDirectionDelta,
      gridPosTranslated + directionDelta,
      gridPosTranslated + directionDelta - complementaryDirectionDelta};

  for (size_t i = 0; i < 3; i++) {
    if (targetNeighbourPositions[i] < 0 &&
        targetNeighbourPositions[i] >= NO_COLUMNS * NO_ROWS) {
      continue;
    }

    if (grid[targetNeighbourPositions[i]] == CELL_OBSTACLE)
      obstaclesNearbyNumber++;
  }

  if (GetRandomValue(0, 999) / 1000.0f <
      spawnThresholds[obstaclesNearbyNumber]) {
    grid[gridPosTranslated] = CELL_OBSTACLE;
  } else
    grid[gridPosTranslated] = 0;
}

void *MarkEdgeObstacleThreaded(void *threadArgs) {
  struct MarkEdgeThreadArgs *args = threadArgs;

  printf("Thread %d: hello world\n", args->id);

  const int gridPosTranslated = args->y * NO_COLUMNS + args->x;
  int obstaclesNearbyNumber = 0;

  int targetNeighbourPositions[3] = {gridPosTranslated + args->directionDelta +
                                         args->complementaryDirectionDelta,
                                     gridPosTranslated + args->directionDelta,
                                     gridPosTranslated + args->directionDelta -
                                         args->complementaryDirectionDelta};

  for (size_t i = 0; i < 3; i++) {
    if (targetNeighbourPositions[i] < 0 &&
        targetNeighbourPositions[i] >= NO_COLUMNS * NO_ROWS) {
      continue;
    }

    if (args->grid[targetNeighbourPositions[i]] == CELL_OBSTACLE)
      obstaclesNearbyNumber++;
  }

  if (GetRandomValue(0, 999) / 1000.0f <
      args->spawnThresholds[obstaclesNearbyNumber]) {
    args->grid[gridPosTranslated] = CELL_OBSTACLE;
  } else
    args->grid[gridPosTranslated] = 0;

  return NULL;
}

void GenerateEdgeObstacles(int *grid, int rowStart, int columnStart, int rowEnd,
                           int columnEnd, int directionDelta,
                           double *averageTimePerCell) {

  float spawnThresholds[4] = {.05f, .2f, .5f, .9f}; // classic
  // float spawnThresholds[4] = {.1f, .4f, .7f, .8f}; // 90% full
  // float spawnThresholds[4] = {.01f, .2f, .5f, .75f}; // sparse
  // float spawnThresholds[4] = {.005f, .1f, .5f, .9f}; // boring
  int numberOfCellsGenerated = NO_COLUMNS;
  int complementaryDirectionDelta = 1;

  if (abs(directionDelta) == 1) {
    numberOfCellsGenerated = NO_ROWS;
    complementaryDirectionDelta = NO_COLUMNS;
  }

  double startEdgeGenerationTime = GetTime();
  pthread_t threads[numberOfCellsGenerated];
  int id = 0;
  int threadResultCode;
  struct MarkEdgeThreadArgs threadsArgs[numberOfCellsGenerated];

  for (size_t y = rowStart; y < rowEnd; y += 1) {
    for (size_t x = columnStart; x < columnEnd; x += 1) {
      switch (THREADING_LEVEL) {
      case 1:
        // hack :(
        {
        } // hack :(
        struct MarkEdgeThreadArgs args = {id,
                                          grid,
                                          x,
                                          y,
                                          directionDelta,
                                          complementaryDirectionDelta,
                                          spawnThresholds};

        threadsArgs[id] = args;

        printf("Main - Creating thread %d\n", id);

        threadResultCode = pthread_create(
            &threads[id], NULL, MarkEdgeObstacleThreaded, &threadsArgs[id]);
        assert(!threadResultCode);
        id++;

        break;
      default:
        MarkEdgeObstacle(grid, x, y, directionDelta,
                         complementaryDirectionDelta, spawnThresholds);
        break;
      }
    }
  }

  if (THREADING_LEVEL == 1) {
    printf("Main - All threads created\n");
    id = 0;

    for (size_t y = rowStart; y < rowEnd; y += 1) {
      for (size_t x = columnStart; x < columnEnd; x += 1) {
        threadResultCode = pthread_join(threads[id], NULL);
        assert(!threadResultCode);
        printf("Main - End thread %d\n", id);
        id++;
      }
    }
  }

  *averageTimePerCell =
      (*averageTimePerCell + (GetTime() - startEdgeGenerationTime) * 1000.0f /
                                 numberOfCellsGenerated) /
      2;
}
