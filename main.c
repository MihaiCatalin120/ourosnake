#include "raylib.h"
#include "raymath.h"
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
#define GRID_CELL_SIZE 50
#define DEBUG_MODE false
#define TIME_PER_TURN .25f
#define CELL_EMPTY 0
#define CELL_OBSTACLE -1
#define CELL_GOAL -2
#define MAIN_PADDING 20
#define TOP_PADDING 60
#define NO_COLUMNS (int)((WINDOW_WIDTH - 2 * MAIN_PADDING) / GRID_CELL_SIZE)
#define NO_ROWS                                                                \
  (int)((WINDOW_HEIGHT - TOP_PADDING - MAIN_PADDING) / GRID_CELL_SIZE)
#define GAME_TITLE "ourosnake"
#define PAUSE_ENABLED false

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
  if (snake->head.x >= (int)NO_COLUMNS)
    snake->head.x = 0;
  if (snake->head.x < 0)
    snake->head.x = (int)NO_COLUMNS - 1;
  if (snake->head.y >= (int)NO_ROWS)
    snake->head.y = 0;
  if (snake->head.y < 0)
    snake->head.y = (int)NO_ROWS - 1;

  if (DEBUG_MODE)
    printf("[DEBUG]: Head is now x:%f y:%f\n", snake->head.x, snake->head.y);
}

void UpdateCellLives(int *grid, struct Snake snake) {
  for (size_t y = 0; y < NO_ROWS; y += 1) {
    for (size_t x = 0; x < NO_COLUMNS; x += 1) {
      const int gridPosTranslated = y * NO_COLUMNS + x;

      if ((int)snake.head.x == x && (int)snake.head.y == y) {
        if (grid[gridPosTranslated] == CELL_EMPTY)
          grid[gridPosTranslated] = snake.length;
      } else if (grid[gridPosTranslated] > 0) {
        grid[gridPosTranslated] -= 1;
      }
    }
  }
}

void PlaySoundWithMuteCheck(Sound sound, bool muted) {
  if (!muted)
    PlaySound(sound);
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

void DrawGameHeader(int *round) {
  char roundText[256];
  sprintf(roundText, "Round %d", *round);
  DrawText(roundText, 10, 10, 24, WHITE);

  const char *title = GAME_TITLE;
  int titleFontSize = 48;
  int titleWidth = MeasureText(title, titleFontSize);
  DrawText(title, WINDOW_WIDTH / 2 - titleWidth / 2, 10, titleFontSize, GREEN);

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
      if (grid[gridPosTranslated] == CELL_GOAL) {
        DrawRectangle(x * GRID_CELL_SIZE + MAIN_PADDING + 1,
                      y * GRID_CELL_SIZE + TOP_PADDING + 1, GRID_CELL_SIZE - 1,
                      GRID_CELL_SIZE - 1, GetColor(0x00FF00FF));
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
      grid[y * NO_COLUMNS + x] = CELL_EMPTY;
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
  while (grid[(int)pos.y * NO_COLUMNS + (int)pos.x] != CELL_EMPTY) {
    pos.x = GetRandomValue(0, NO_COLUMNS - 1);
    pos.y = GetRandomValue(0, NO_ROWS - 1);
  }

  grid[(int)pos.y * NO_COLUMNS + (int)pos.x] = CELL_GOAL;
}

int main() {
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, GAME_TITLE);
  InitAudioDevice();
  // SetTargetFPS(60);
  int *grid = (int *)MemAlloc(NO_COLUMNS * NO_ROWS * sizeof(int));
  int currentRound = 1;
  Vector2 head, initialDirection;
  struct Snake snake;
  float time;
  bool gameOver, roundWon, restart, muted, paused;
  Sound stepWav = LoadSound("assets/audio/step.wav");
  Sound winWav = LoadSound("assets/audio/win.wav");
  Sound loseWav = LoadSound("assets/audio/lose.wav");
  Image mutedIcon = LoadImage("assets/icons/volume-mute-line.png");
  Texture2D mutedTexture = LoadTextureFromImage(mutedIcon);
  UnloadImage(mutedIcon);

  restart = true;
  muted = false;
  paused = false;
  printf("Number of rows [%d] columns [%d]\n", NO_ROWS, NO_COLUMNS);
  printf("Entering main game loop\n");
  while (!WindowShouldClose()) {
    if (restart) {
      ClearGrid(grid);

      // TODO: find formula to ensure obstacles are not crowding
      // the grid / grid is not too empty
      // printf("Generating obstacles\n");
      GenerateInitialObstacles(grid, 5);
      GenerateGoal(grid);

      head.x = GetRandomValue(0, NO_COLUMNS - 1);
      head.y = GetRandomValue(0, NO_ROWS - 1);
      // Avoid spawning directly on an obstacle
      // TODO: find a way to ensure obstacles are at least 2 spaces away from
      // the snake head spawn point
      while (grid[(int)head.y * NO_COLUMNS + (int)head.x] != CELL_EMPTY) {
        head.x = GetRandomValue(0, NO_COLUMNS - 1);
        head.y = GetRandomValue(0, NO_ROWS - 1);
      }

      initialDirection = directions[GetRandomValue(0, 3)];
      snake.head = head;
      snake.direction = initialDirection;
      snake.length = 5;
      time = 0;
      gameOver = false;
      roundWon = false;
      restart = false;
    }

    if (!gameOver && !roundWon && !paused) {
      float dt = GetFrameTime();
      time += dt;
    }

    CheckForDirectionChange(&snake);

    if (gameOver) {
      if (IsKeyDown(KEY_R)) {
        ClearGrid(grid);
        currentRound = 1;
        gameOver = false;
        restart = true;
      }
    }

    if (roundWon) {
      if (IsKeyDown(KEY_ENTER)) {
        ClearGrid(grid);
        currentRound++;
        roundWon = false;
        restart = true;
      }
    }

    if (IsKeyPressed(KEY_M)) {
      muted = !muted;
    }

    if (IsKeyPressed(KEY_P) && PAUSE_ENABLED) {
      paused = !paused;
    }

    if (time > TIME_PER_TURN && !gameOver && !roundWon && !paused) {
      // printf("Periodic game update\n");
      time -= TIME_PER_TURN;
      // printf("Update snake position\n");
      UpdateSnakePosition(grid, &snake);
      // printf("Check for final round states\n");
      if (grid[(int)snake.head.y * NO_COLUMNS + (int)snake.head.x] ==
          CELL_GOAL) {
        roundWon = true;
        PlaySoundWithMuteCheck(winWav, muted);
        goto draw;
      } else if (grid[(int)snake.head.y * NO_COLUMNS + (int)snake.head.x] !=
                 CELL_EMPTY) {
        gameOver = true;
        PlaySoundWithMuteCheck(loseWav, muted);
        goto draw;
      }
      // printf("Updating rest of grid\n");
      UpdateCellLives(grid, snake);
      PlaySoundWithMuteCheck(stepWav, muted);
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
      DrawGameHeader(&currentRound);
      if (roundWon) {
        // printf("Drawing round won box\n");
        DrawEndRoundBox("Round Won", "Press enter to continue", 64, 12, GREEN);
      }
      if (gameOver) {
        // printf("Drawing game over box\n");
        DrawEndRoundBox("Game Over", "Press R to restart", 64, 12, RED);
      }
      if (paused) {
        // printf("Drawing game over box\n");
        DrawEndRoundBox("Paused", "Press P to resume", 64, 12, BLUE);
      }
      if (muted) {
        DrawTexture(mutedTexture, WINDOW_WIDTH - 30, 30, RED);
      }
      EndDrawing();
    }
  }

  // printf("Deallocating\n");
  CloseAudioDevice();
  UnloadTexture(mutedTexture);
  CloseWindow();
  MemFree(grid);
  return 0;
}
