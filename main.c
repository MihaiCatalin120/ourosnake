#include "raylib.h"
#include "raymath.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// WARN: If NO_COLUMNS or NO_ROWS have floating points the game will not work
// properly
#define WINDOW_WIDTH 840
#define WINDOW_HEIGHT 680
#define GRID_CELL_SIZE 25
#define DEBUG_MODE true
#define TIME_PER_TURN .25f
#define CELL_OBSTACLE -1
#define CELL_GOAL -2
#define MAIN_PADDING 20
#define TOP_PADDING 60
#define NO_COLUMNS (int)((WINDOW_WIDTH - 2 * MAIN_PADDING) / GRID_CELL_SIZE)
#define NO_ROWS                                                                \
  (int)((WINDOW_HEIGHT - TOP_PADDING - MAIN_PADDING) / GRID_CELL_SIZE)
#define GRID_SHIFT_THRESHOLD 3
#define GAME_TITLE "ourosnake"

struct Snake {
  Vector2 head;
  Vector2 direction;
  int length;
};

struct ObstacleGenerator {
  Vector2 startPosition;
  Vector2 moves[10]; // TODO: support dynamic moves max size
};

// Up, Right, Down, Left
Vector2 directions[4] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};

void UpdateSnakePosition(int *grid, struct Snake *snake) {
  // Make the step
  snake->head = Vector2Add(snake->head, snake->direction);

  if (DEBUG_MODE)
    printf("[DEBUG]: Head is now x:%f y:%f\n", snake->head.x, snake->head.y);
}

void UpdateCellLives(int *grid, struct Snake snake) {
  for (size_t y = 0; y < NO_ROWS; y += 1) {
    for (size_t x = 0; x < NO_COLUMNS; x += 1) {
      const int gridPosTranslated = y * NO_COLUMNS + x;

      if ((int)snake.head.x == x && (int)snake.head.y == y) {
        if (grid[gridPosTranslated] == 0)
          grid[gridPosTranslated] = snake.length;
      } else if (grid[gridPosTranslated] > 0) {
        grid[gridPosTranslated] -= 1;
      }
    }
  }
}

void DrawGrid2D() {
  for (size_t x = 0; x <= NO_COLUMNS; x += 1) {
    DrawLine(x * GRID_CELL_SIZE + MAIN_PADDING, TOP_PADDING,
             x * GRID_CELL_SIZE + MAIN_PADDING, WINDOW_HEIGHT - MAIN_PADDING,
             GetColor(0xFFFFFF20));
  }

  for (size_t y = 0; y <= NO_ROWS; y += 1) {
    DrawLine(MAIN_PADDING, y * GRID_CELL_SIZE + TOP_PADDING,
             WINDOW_WIDTH - MAIN_PADDING, y * GRID_CELL_SIZE + TOP_PADDING,
             GetColor(0xFFFFFF20));
  }
}

void DrawGameHeader() {
  int cells = 10;
  float time = 2.23f;
  char info[256];
  sprintf(info, "Generated %d cells in %f ms", cells, time);
  DrawText(info, 10, 10, 24, WHITE);

  DrawFPS(WINDOW_WIDTH - 100, 10);
}

void DrawDebugCellValues(int *grid) {
  for (size_t y = 0; y < NO_ROWS; y += 1) {
    for (size_t x = 0; x < NO_COLUMNS; x += 1) {
      const int gridPosTranslated = y * NO_COLUMNS + x;

      if (DEBUG_MODE) {
        // Debug text to show grid values, comment if not used
        char text[4];
        int fontSize = Clamp(GRID_CELL_SIZE, 10, 64);
        sprintf(text, "%d", grid[gridPosTranslated]);
        DrawText(text, x * GRID_CELL_SIZE + MAIN_PADDING + GRID_CELL_SIZE / 4,
                 y * GRID_CELL_SIZE + TOP_PADDING + GRID_CELL_SIZE / 2 -
                     fontSize / 2,
                 fontSize, GetColor(0x000000FF));
        // End debug
      }
    }
  }
}

void DrawObjects(int *grid) {
  for (size_t y = 0; y < NO_ROWS; y += 1) {
    for (size_t x = 0; x < NO_COLUMNS; x += 1) {
      const int gridPosTranslated = y * NO_COLUMNS + x;

      // Snake
      if (grid[gridPosTranslated] > 0) {
        DrawRectangle(x * GRID_CELL_SIZE + MAIN_PADDING + 1,
                      y * GRID_CELL_SIZE + TOP_PADDING + 1, GRID_CELL_SIZE - 1,
                      GRID_CELL_SIZE - 1, GetColor(0x00FFFFFF));
      }

      // Obstacle
      if (grid[gridPosTranslated] == CELL_OBSTACLE) {
        DrawRectangle(x * GRID_CELL_SIZE + MAIN_PADDING + 1,
                      y * GRID_CELL_SIZE + TOP_PADDING + 1, GRID_CELL_SIZE - 1,
                      GRID_CELL_SIZE - 1, GetColor(0xFFFFFFFF));
      }

      // Goal
      // if (grid[gridPosTranslated] == CELL_GOAL) {
      //   DrawRectangle(x * GRID_CELL_SIZE + MAIN_PADDING + 1,
      //                 y * GRID_CELL_SIZE + TOP_PADDING + 1, GRID_CELL_SIZE -
      //                 1, GRID_CELL_SIZE - 1, GetColor(0x00FF00FF));
      // }
    }
  }
}

bool IsValueInArray(int value, int array[], int length) {
  for (size_t i = 0; i < length; i++) {
    if (value == array[i])
      return true;
  }

  return false;
}

void CheckForDirectionChange(struct Snake *snake) {
  int key = GetKeyPressed();

  if (key == KEY_NULL)
    return;

  int upCases[2] = {KEY_UP, KEY_W};
  int rightCases[2] = {KEY_RIGHT, KEY_D};
  int downCases[2] = {KEY_DOWN, KEY_S};
  int leftCases[2] = {KEY_LEFT, KEY_A};

  if (!Vector2Equals(snake->direction, directions[2]) &&
      IsValueInArray(key, upCases, sizeof(upCases) / sizeof(upCases[0]))) {
    snake->direction = directions[0];
    return;
  }

  if (!Vector2Equals(snake->direction, directions[3]) &&
      IsValueInArray(key, rightCases,
                     sizeof(rightCases) / sizeof(rightCases[0]))) {
    snake->direction = directions[1];
    return;
  }

  if (!Vector2Equals(snake->direction, directions[0]) &&
      IsValueInArray(key, downCases,
                     sizeof(downCases) / sizeof(downCases[0]))) {
    snake->direction = directions[2];
    return;
  }

  if (!Vector2Equals(snake->direction, directions[1]) &&
      IsValueInArray(key, leftCases,
                     sizeof(leftCases) / sizeof(leftCases[0]))) {
    snake->direction = directions[3];
    return;
  }
}

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

void ClearGrid(int *grid) {
  for (size_t y = 0; y < NO_ROWS; y += 1) {
    for (size_t x = 0; x < NO_COLUMNS; x += 1) {
      grid[y * NO_COLUMNS + x] = 0;
    }
  }
}

void DrawEndRoundBox(const char mainText[], const char secondaryText[],
                     int mainFontSize, int secondaryFontSize,
                     Color mainTextColor) {
  int mainTextWidth = MeasureText(mainText, mainFontSize);
  int secondaryTextWidth = MeasureText(secondaryText, secondaryFontSize);
  DrawRectangle(WINDOW_WIDTH / 2 - mainTextWidth / 2 - 20,
                WINDOW_HEIGHT / 2 - mainFontSize / 2 - 20, mainTextWidth + 40,
                mainFontSize + secondaryFontSize + 40 + 10, BLACK);
  DrawText(mainText, WINDOW_WIDTH / 2 - mainTextWidth / 2,
           WINDOW_HEIGHT / 2 - mainFontSize / 2, mainFontSize, mainTextColor);
  DrawText(secondaryText, WINDOW_WIDTH / 2 - secondaryTextWidth / 2,
           WINDOW_HEIGHT / 2 - secondaryFontSize / 2 + mainFontSize / 2 + 10,
           secondaryFontSize, WHITE);
}

// TODO: Use vector distance to ensure goal is not too near of the starting
// position
void GenerateGoal(int *grid) {
  Vector2 pos = {GetRandomValue(0, NO_COLUMNS - 1),
                 GetRandomValue(0, NO_ROWS - 1)};
  // Avoid spawning directly on an obstacle or snake
  while (grid[(int)pos.y * NO_COLUMNS + (int)pos.x] != 0) {
    pos.x = GetRandomValue(0, NO_COLUMNS - 1);
    pos.y = GetRandomValue(0, NO_ROWS - 1);
  }

  // printf("Spawning goal at [%d][%d]\n", (int)pos.x, (int)pos.y);
  grid[(int)pos.y * NO_COLUMNS + (int)pos.x] = CELL_GOAL;
}

void GenerateEdgeObstacles(int *grid, int rowStart, int columnStart, int rowEnd,
                           int columnEnd, int directionDelta) {

  float spawnThresholds[4] = {.05f, .2f, .5f, .9f};

  for (size_t y = rowStart; y < rowEnd; y += 1) {
    for (size_t x = columnStart; x < columnEnd; x += 1) {
      const int gridPosTranslated = y * NO_COLUMNS + x;
      int obstaclesNearbyNumber = 0;
      int complementaryDirectionDelta = 1;

      if (abs(directionDelta) == 1)
        complementaryDirectionDelta = NO_COLUMNS;

      int targetNeighbourPositions[3] = {
          gridPosTranslated + directionDelta + complementaryDirectionDelta,
          gridPosTranslated + directionDelta,
          gridPosTranslated + directionDelta - complementaryDirectionDelta};

      for (size_t i = 0; i < 3; i++) {
        if (targetNeighbourPositions[i] >= 0 &&
            targetNeighbourPositions[i] < NO_COLUMNS * NO_ROWS) {
          continue;
        }

        if (grid[targetNeighbourPositions[i]] == CELL_OBSTACLE)
          obstaclesNearbyNumber++;
      }

      if (GetRandomValue(0, 99) / 100.0f <
          spawnThresholds[obstaclesNearbyNumber]) {
        grid[gridPosTranslated] = CELL_OBSTACLE;
      }
    }
  }
}

void ShiftGridIfNeeded(int *grid, struct Snake *snake) {
  int directionDelta = 0;
  int rowStart = 0;
  int columnStart = 0;
  int rowEnd = NO_ROWS;
  int columnEnd = NO_COLUMNS;
  // Right shift - Left column needs fill
  if (snake->head.x < GRID_SHIFT_THRESHOLD) {
    columnEnd = 1;
    directionDelta = 1;
  }
  // Left shift - Right column needs fill
  if (snake->head.x >= NO_COLUMNS - GRID_SHIFT_THRESHOLD - 1) {
    columnStart = NO_COLUMNS - 1;
    directionDelta = -1;
  }
  // Down shift - Top column needs fill
  if (snake->head.y < GRID_SHIFT_THRESHOLD) {
    rowEnd = 1;
    directionDelta = NO_COLUMNS;
  }
  // Up shift - Bottom column needs fill
  if (snake->head.y >= NO_ROWS - GRID_SHIFT_THRESHOLD - 1) {
    rowStart = NO_ROWS - 1;
    directionDelta = -NO_COLUMNS;
  }

  // TODO: Snake position should be updated even if direction delta is not 0,
  // e.g. running along the threshold edge
  if (directionDelta == 0) {
    UpdateSnakePosition(grid, snake);
    return;
  }

  for (size_t y = 0; y < NO_ROWS; y += 1) {
    for (size_t x = 0; x < NO_COLUMNS; x += 1) {
      const int gridPosTranslated = y * NO_COLUMNS + x;
      const int targetPos = gridPosTranslated + directionDelta;
      if (targetPos < 0 || targetPos >= NO_COLUMNS * NO_ROWS)
        continue;

      grid[targetPos] = grid[gridPosTranslated];
    }
  }

  GenerateEdgeObstacles(grid, rowStart, columnStart, rowEnd, columnEnd,
                        directionDelta);
}

int main() {
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, GAME_TITLE);
  InitAudioDevice();
  // SetTargetFPS(60);
  // printf("Main init \n");
  int *grid = (int *)MemAlloc(NO_COLUMNS * NO_ROWS * sizeof(int));
  Vector2 head, initialDirection;
  struct Snake snake;
  float time;
  bool gameOver, restart;
  Sound stepWav = LoadSound("assets/audio/step.wav");
  Sound winWav = LoadSound("assets/audio/win.wav");
  Sound loseWav = LoadSound("assets/audio/lose.wav");

  restart = true;
  printf("Number of rows [%d] columns [%d]\n", NO_ROWS, NO_COLUMNS);
  printf("Entering main game loop\n");
  while (!WindowShouldClose()) {
    if (restart) {
      // printf("Classic init \n");
      // printf("Clearing grid \n");
      ClearGrid(grid);

      // TODO: find formula to ensure obstacles are not crowding
      // the grid / grid is not too empty
      // printf("Generating obstacles\n");
      GenerateInitialObstacles(grid, 5);
      // printf("Generating goal\n");
      // GenerateGoal(grid);

      // printf("Generating snake head\n");
      head.x = GetRandomValue(GRID_SHIFT_THRESHOLD,
                              NO_COLUMNS - 1 - GRID_SHIFT_THRESHOLD);
      head.y = GetRandomValue(GRID_SHIFT_THRESHOLD,
                              NO_ROWS - 1 - GRID_SHIFT_THRESHOLD);
      // Avoid spawning directly on an obstacle
      // TODO: find a way to ensure obstacles are at least 2 spaces away from
      // the snake head spawn point
      while (grid[(int)head.y * NO_COLUMNS + (int)head.x] != 0) {
        head.x = GetRandomValue(GRID_SHIFT_THRESHOLD,
                                NO_COLUMNS - 1 - GRID_SHIFT_THRESHOLD);
        head.y = GetRandomValue(GRID_SHIFT_THRESHOLD,
                                NO_ROWS - 1 - GRID_SHIFT_THRESHOLD);
      }

      // printf("Setting snake head at [%d][%d] = %d\n", (int)head.x,
      // (int)head.y,
      //        grid[(int)head.y * WINDOW_WIDTH + (int)head.x]);
      // printf("Assigning init variable values\n");
      initialDirection = directions[GetRandomValue(0, 3)];
      snake.head = head;
      snake.direction = initialDirection;
      snake.length = 5;
      time = 0;
      gameOver = false;
      restart = false;
    }

    if (!gameOver) {
      float dt = GetFrameTime();
      time += dt;
    }

    // printf("Checking inputs\n");
    CheckForDirectionChange(&snake);

    // printf("Checking end game states\n");
    if (gameOver) {
      if (IsKeyDown(KEY_R)) {
        // printf("Game over, r pressed\n");
        ClearGrid(grid);
        gameOver = false;
        restart = true;
      }
    }

    if (time > TIME_PER_TURN && !gameOver) {
      // printf("Periodic game update\n");
      time -= TIME_PER_TURN;
      ShiftGridIfNeeded(grid, &snake);
      // printf("Check for final round states\n");
      if (grid[(int)snake.head.y * NO_COLUMNS + (int)snake.head.x] != 0) {
        gameOver = true;
        PlaySound(loseWav);
        goto draw;
      }
      // printf("Updating rest of grid\n");
      UpdateCellLives(grid, snake);
      PlaySound(stepWav);
    }
    // if (DEBUG_MODE) {
    //   printf("[DEBUG]: Delta - %f\n", dt);
    //   printf("[DEBUG]: Time - %f\n", time);
    // }
  draw:
    if (!restart) {
      // printf("Draw start\n");
      BeginDrawing();
      ClearBackground(GetColor(0x202020FF));

      // printf("Drawing objects\n");
      DrawObjects(grid);
      if (DEBUG_MODE) {
        DrawDebugCellValues(grid);
      }
      // printf("Drawing grid\n");
      DrawGrid2D();
      DrawGameHeader();
      if (gameOver) {
        // printf("Drawing game over box\n");
        DrawEndRoundBox("Game Over", "Press R to restart", 64, 12, RED);
      }
      EndDrawing();
    }
  }

  // printf("Deallocating\n");
  CloseAudioDevice();
  CloseWindow();
  MemFree(grid);
  return 0;
}
