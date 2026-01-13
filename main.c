#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define GRID_CELL_SIZE 50
#define DEBUG_MODE false
#define TIME_PER_TURN 0.5f
#define CELL_OBSTACLE -1
#define CELL_GOAL -2

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

  // Wrap head position if outside of grid
  if (snake->head.x >= WINDOW_WIDTH * 1.0f / GRID_CELL_SIZE)
    snake->head.x = 0;
  if (snake->head.x < 0)
    snake->head.x = WINDOW_WIDTH * 1.0f / GRID_CELL_SIZE - 1;
  if (snake->head.y >= WINDOW_HEIGHT * 1.0f / GRID_CELL_SIZE)
    snake->head.y = 0;
  if (snake->head.y < 0)
    snake->head.y = WINDOW_HEIGHT * 1.0f / GRID_CELL_SIZE - 1;

  if (DEBUG_MODE)
    printf("[DEBUG]: Head is now x:%f y:%f\n", snake->head.x, snake->head.y);
}

void UpdateCellLives(int *grid, struct Snake snake) {
  for (size_t y = 0; y < WINDOW_HEIGHT / GRID_CELL_SIZE; y += 1) {
    for (size_t x = 0; x < WINDOW_WIDTH / GRID_CELL_SIZE; x += 1) {
      const int gridPosTranslated = y * WINDOW_WIDTH + x;

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
  for (size_t x = 0; x < WINDOW_WIDTH / GRID_CELL_SIZE; x += 1) {
    DrawLine(x * GRID_CELL_SIZE, 0, x * GRID_CELL_SIZE, WINDOW_HEIGHT,
             GetColor(0xFFFFFF20));
  }

  for (size_t y = 0; y < WINDOW_HEIGHT / GRID_CELL_SIZE; y += 1) {
    DrawLine(0, y * GRID_CELL_SIZE, WINDOW_WIDTH, y * GRID_CELL_SIZE,
             GetColor(0xFFFFFF20));
  }
}

void DrawDebugCellValues(int *grid) {
  for (size_t y = 0; y < WINDOW_HEIGHT / GRID_CELL_SIZE; y += 1) {
    for (size_t x = 0; x < WINDOW_WIDTH / GRID_CELL_SIZE; x += 1) {
      const int gridPosTranslated = y * WINDOW_WIDTH + x;

      if (DEBUG_MODE) {
        // Debug text to show grid values, comment if not used
        char text[4];
        int fontSize = Clamp(GRID_CELL_SIZE, 10, 64);
        sprintf(text, "%d", grid[gridPosTranslated]);
        DrawText(text, x * GRID_CELL_SIZE + GRID_CELL_SIZE / 4,
                 y * GRID_CELL_SIZE + GRID_CELL_SIZE / 2 - fontSize / 2,
                 fontSize, GetColor(0x000000FF));
        // End debug
      }
    }
  }
}

void DrawObjects(int *grid) {
  for (size_t y = 0; y < WINDOW_HEIGHT / GRID_CELL_SIZE; y += 1) {
    for (size_t x = 0; x < WINDOW_WIDTH / GRID_CELL_SIZE; x += 1) {
      const int gridPosTranslated = y * WINDOW_WIDTH + x;

      // Snake
      if (grid[gridPosTranslated] > 0) {
        DrawRectangle(x * GRID_CELL_SIZE + 1, y * GRID_CELL_SIZE + 1,
                      GRID_CELL_SIZE - 1, GRID_CELL_SIZE - 1,
                      GetColor(0x00FFFFFF));
      }

      // Obstacle
      if (grid[gridPosTranslated] == CELL_OBSTACLE) {
        DrawRectangle(x * GRID_CELL_SIZE + 1, y * GRID_CELL_SIZE + 1,
                      GRID_CELL_SIZE - 1, GRID_CELL_SIZE - 1,
                      GetColor(0xFFFFFFFF));
      }

      // Goal
      if (grid[gridPosTranslated] == CELL_GOAL) {
        DrawRectangle(x * GRID_CELL_SIZE + 1, y * GRID_CELL_SIZE + 1,
                      GRID_CELL_SIZE - 1, GRID_CELL_SIZE - 1,
                      GetColor(0x00FF00FF));
      }
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

  if (IsValueInArray(key, upCases, sizeof(upCases) / sizeof(upCases[0]))) {
    snake->direction = directions[0];
    return;
  }

  if (IsValueInArray(key, rightCases,
                     sizeof(rightCases) / sizeof(rightCases[0]))) {
    snake->direction = directions[1];
    return;
  }

  if (IsValueInArray(key, downCases,
                     sizeof(downCases) / sizeof(downCases[0]))) {
    snake->direction = directions[2];
    return;
  }

  if (IsValueInArray(key, leftCases,
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
    grid[(int)head.y * WINDOW_WIDTH + (int)head.x] = CELL_OBSTACLE;
    head = Vector2Add(head, generator.moves[index]);
    index++;
  }
}

void GenerateInitialObstacles(int *grid, int numberOfObstacles) {
  Vector2 endDirection = {0, 0};

  for (size_t i = 0; i < numberOfObstacles; i++) {
    struct ObstacleGenerator generator;

    generator.startPosition.x =
        (int)GetRandomValue(0, WINDOW_WIDTH / GRID_CELL_SIZE);
    generator.startPosition.y =
        (int)GetRandomValue(0, WINDOW_HEIGHT / GRID_CELL_SIZE);

    size_t moveNumber;
    for (moveNumber = 0; moveNumber < GetRandomValue(3, 6); moveNumber++) {
      generator.moves[moveNumber] = directions[GetRandomValue(0, 3)];
    }
    generator.moves[moveNumber] = endDirection;

    MarkObstacle(grid, generator);
  }
}

void ClearGrid(int *grid) {
  for (size_t y = 0; y < WINDOW_HEIGHT / GRID_CELL_SIZE; y += 1) {
    for (size_t x = 0; x < WINDOW_WIDTH / GRID_CELL_SIZE; x += 1) {
      grid[y * WINDOW_WIDTH + x] = 0;
    }
  }
}

void DrawEndRoundBox(const char mainText[], const char secondaryText[],
                     int mainFontSize, int secondaryFontSize,
                     Color mainTextColor) {
  int mainTextWidth = MeasureText(mainText, mainFontSize);
  int secondaryTextWidth = MeasureText(secondaryText, secondaryTextWidth);
  DrawRectangle(WINDOW_WIDTH / 2 - mainTextWidth / 2 - 20,
                WINDOW_HEIGHT / 2 - mainFontSize / 2 - 20, mainTextWidth + 40,
                mainFontSize + secondaryFontSize + 40 + 10, BLACK);
  DrawText(mainText, WINDOW_WIDTH / 2 - mainTextWidth / 2,
           WINDOW_HEIGHT / 2 - mainFontSize / 2, mainFontSize, mainTextColor);
  DrawText(secondaryText, WINDOW_WIDTH / 2 - secondaryTextWidth / 2,
           WINDOW_HEIGHT / 2 - secondaryFontSize / 2 + mainFontSize / 2 + 10,
           secondaryFontSize, WHITE);
}

void GenerateGoal(int *grid) {
  Vector2 pos = {GetRandomValue(0, WINDOW_WIDTH / GRID_CELL_SIZE),
                 GetRandomValue(0, WINDOW_HEIGHT / GRID_CELL_SIZE)};
  // Avoid spawning directly on an obstacle or snake
  while (grid[(int)pos.y * WINDOW_WIDTH + (int)pos.x] != 0) {
    pos.x = GetRandomValue(0, WINDOW_WIDTH / GRID_CELL_SIZE);
    pos.y = GetRandomValue(0, WINDOW_HEIGHT / GRID_CELL_SIZE);
  }

  grid[(int)pos.y * WINDOW_WIDTH + (int)pos.x] = CELL_GOAL;
}

// TODO: Configure LSP to work on current folder structure without any hacks
// (currently duplicating header files into source code directory just for LSP)

// TODO Sometimes no end goal is spawned - needs investigation
//
// TODO segfault after >5 rounds - check
int main() {
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "ourosnake");
  InitAudioDevice();
  SetTargetFPS(60);
  int *grid = (int *)MemAlloc(WINDOW_WIDTH * WINDOW_HEIGHT / GRID_CELL_SIZE *
                              sizeof(int));
init:
  ClearGrid(grid);

  // TODO: find formula to ensure obstacles are not crowding
  // the grid / grid is not too empty
  GenerateInitialObstacles(grid, 5);
  GenerateGoal(grid);

  Vector2 head = {GetRandomValue(0, WINDOW_WIDTH / GRID_CELL_SIZE),
                  GetRandomValue(0, WINDOW_HEIGHT / GRID_CELL_SIZE)};
  // Avoid spawning directly on an obstacle
  while (grid[(int)head.y * WINDOW_WIDTH + (int)head.x] != 0) {
    head.x = GetRandomValue(0, WINDOW_WIDTH / GRID_CELL_SIZE);
    head.y = GetRandomValue(0, WINDOW_HEIGHT / GRID_CELL_SIZE);
  }
  Vector2 initialDirection = directions[GetRandomValue(0, 3)];
  struct Snake snake = {head, initialDirection, 5};
  float time = 0;
  bool gameOver = false;
  bool roundWon = false;

  while (!WindowShouldClose()) {
    if (!gameOver && !roundWon) {
      float dt = GetFrameTime();
      time += dt;
    }

    CheckForDirectionChange(&snake);

    // TODO: refactor into separate function
    if (gameOver) {
      if (IsKeyDown(KEY_R)) {
        ClearGrid(grid);
        gameOver = false;
        goto init;
      }
    }

    if (roundWon) {
      // TODO: change to all keys
      if (IsKeyDown(KEY_ENTER)) {
        ClearGrid(grid);
        roundWon = false;
        goto init;
      }
    }

    if (time > TIME_PER_TURN && !gameOver) {
      time -= TIME_PER_TURN;
      UpdateSnakePosition(grid, &snake);
      if (grid[(int)snake.head.y * WINDOW_WIDTH + (int)snake.head.x] ==
          CELL_GOAL) {
        roundWon = true;
        goto draw;
      } else if (grid[(int)snake.head.y * WINDOW_WIDTH + (int)snake.head.x] !=
                 0) {
        gameOver = true;
        goto draw;
      }
      UpdateCellLives(grid, snake);
    }
    // if (DEBUG_MODE) {
    //   printf("[DEBUG]: Delta - %f\n", dt);
    //   printf("[DEBUG]: Time - %f\n", time);
    // }
  draw:
    BeginDrawing();
    ClearBackground(GetColor(0x202020FF));
    DrawObjects(grid);
    if (DEBUG_MODE) {
      DrawFPS(700, 100);
      DrawDebugCellValues(grid);
    }
    DrawGrid2D();
    if (roundWon) {
      DrawEndRoundBox("Round Won", "Press any key to continue", 64, 12, GREEN);
    }
    if (gameOver) {
      DrawEndRoundBox("Game Over", "Press R to restart", 64, 12, RED);
    }
    EndDrawing();
  }

  CloseAudioDevice();
  CloseWindow();
  MemFree(grid);
  return 0;
}
