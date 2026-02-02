#include "ui.h"
#include "config.h"
#include "grid.h"
#include "raymath.h"
#include "utils.h"
#include <stdio.h>

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

      char text[4];
      int fontSize = Clamp(GRID_CELL_SIZE, 10, 64);
      sprintf(text, "%d", grid[gridPosTranslated]);
      DrawText(text, x * GRID_CELL_SIZE + MAIN_PADDING + GRID_CELL_SIZE / 4,
               y * GRID_CELL_SIZE + TOP_PADDING + GRID_CELL_SIZE / 2 -
                   fontSize / 2,
               fontSize, GetColor(0x000000FF));
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

void HandleInputs(int *grid, struct Snake *snake, bool *gameOver,
                  int *currentRound, bool *restart, bool *roundWon, bool *muted,
                  bool *paused) {
  if (*gameOver) {
    if (IsKeyDown(KEY_R)) {
      ClearGrid(grid);
      *currentRound = 1;
      *gameOver = false;
      *restart = true;
    }
  }

  if (roundWon) {
    if (IsKeyDown(KEY_ENTER)) {
      ClearGrid(grid);
      *currentRound = *currentRound + 1;
      *roundWon = false;
      *restart = true;
    }
  }

  if (IsKeyPressed(KEY_M)) {
    *muted = !*muted;
  }

  if (IsKeyPressed(KEY_P) && PAUSE_ENABLED) {
    *paused = !*paused;
  }

  CheckForDirectionChange(snake);
}
