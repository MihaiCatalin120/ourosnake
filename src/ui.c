#include "ui.h"
#include "../lib/reasings.h"
#include "config.h"
#include "grid.h"
#include "raylib.h"
#include "raymath.h"
#include "stdlib.h"
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

void DrawGameHeader(int *round, Texture2D logoTexture, Rectangle logoRec) {
  char roundText[256];
  sprintf(roundText, "Round %d", *round);
  DrawText(roundText, 10, 10, 24, WHITE);

  const char *title = GAME_TITLE;
  title++;
  int titleFontSize = 48;
  int titleWidth = MeasureText(title, titleFontSize);
  DrawText(title,
           WINDOW_WIDTH / 2.0f - (titleWidth + logoRec.width) / 2.0f +
               logoRec.width,
           10, titleFontSize, GREEN);

  DrawTextureRec(
      logoTexture, logoRec,
      (Vector2){WINDOW_WIDTH / 2.0f - (titleWidth + logoRec.width) / 2.0f, 10},
      WHITE);

  const char *mutedText = "Mute (M)";
  int mutedTextSize = 16;
  int mutedTextWidth = MeasureText(mutedText, mutedTextSize);
  DrawText("Mute (M)", WINDOW_WIDTH - mutedTextWidth - 40, 14, mutedTextSize,
           WHITE);

  if (DEBUG_MODE)
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

void FillSnakeEdge(int x, int y, int *grid, Color color) {
  Vector2 current = {x, y};
  for (size_t i = 0; i < 4; i++) {
    Vector2 neighbour = Vector2Add(current, directions[i]);
    if (neighbour.x < 0 || neighbour.x >= NO_COLUMNS || neighbour.y < 0 ||
        neighbour.y >= NO_ROWS)
      continue;

    const int currentPosTranslated = y * NO_COLUMNS + x;
    const int neighbourPosTranslated = neighbour.y * NO_COLUMNS + neighbour.x;
    if (grid[neighbourPosTranslated] > 0 &&
        abs(grid[currentPosTranslated] - grid[neighbourPosTranslated]) == 1) {
      switch (i) {
      case 0:
        DrawRectangle(x * GRID_CELL_SIZE + MAIN_PADDING + SNAKE_PADDING,
                      y * GRID_CELL_SIZE + TOP_PADDING,
                      GRID_CELL_SIZE - 2 * SNAKE_PADDING, SNAKE_PADDING, color);
        break;
      case 1:
        DrawRectangle(x * GRID_CELL_SIZE + MAIN_PADDING + GRID_CELL_SIZE -
                          SNAKE_PADDING,
                      y * GRID_CELL_SIZE + TOP_PADDING + SNAKE_PADDING,
                      SNAKE_PADDING, GRID_CELL_SIZE - 2 * SNAKE_PADDING, color);
        break;
      case 2:
        DrawRectangle(x * GRID_CELL_SIZE + MAIN_PADDING + SNAKE_PADDING,
                      y * GRID_CELL_SIZE + TOP_PADDING + GRID_CELL_SIZE -
                          SNAKE_PADDING,
                      GRID_CELL_SIZE - 2 * SNAKE_PADDING, SNAKE_PADDING, color);
        break;
      case 3:
        DrawRectangle(x * GRID_CELL_SIZE + MAIN_PADDING,
                      y * GRID_CELL_SIZE + TOP_PADDING + SNAKE_PADDING,
                      SNAKE_PADDING, GRID_CELL_SIZE - 2 * SNAKE_PADDING, color);
        break;
      }
    }
  }
}

void DrawObjects(int *grid, int frameCounter, bool isGoalAvailable,
                 int goalRequirementCounter) {
  for (size_t y = 0; y < NO_ROWS; y += 1) {
    for (size_t x = 0; x < NO_COLUMNS; x += 1) {
      const int gridPosTranslated = y * NO_COLUMNS + x;

      // Snake (body)
      if (grid[gridPosTranslated] > 0) {
        Color color = GetColor(0x00FFFFFF);
        DrawRectangle(x * GRID_CELL_SIZE + MAIN_PADDING + SNAKE_PADDING,
                      y * GRID_CELL_SIZE + TOP_PADDING + SNAKE_PADDING,
                      GRID_CELL_SIZE - 2 * SNAKE_PADDING,
                      GRID_CELL_SIZE - 2 * SNAKE_PADDING, color);
        FillSnakeEdge(x, y, grid, color);
      }

      // Obstacle
      if (grid[gridPosTranslated] == CELL_OBSTACLE) {
        DrawRectangle(x * GRID_CELL_SIZE + MAIN_PADDING,
                      y * GRID_CELL_SIZE + TOP_PADDING, GRID_CELL_SIZE,
                      GRID_CELL_SIZE, GetColor(0xFFFFFFFF));
      }

      // Goal
      if (grid[gridPosTranslated] == CELL_GOAL) {
        Color color = GetColor(0x00FF00FF);
        if (isGoalAvailable) {
          DrawRectangle(x * GRID_CELL_SIZE + MAIN_PADDING + SNAKE_PADDING,
                        y * GRID_CELL_SIZE + TOP_PADDING + SNAKE_PADDING,
                        GRID_CELL_SIZE - 2 * SNAKE_PADDING,
                        GRID_CELL_SIZE - 2 * SNAKE_PADDING, color);
        } else {
          // Snake (tail non-active)
          color = GetColor(0x00FFFFFF);
          DrawRectangle(x * GRID_CELL_SIZE + MAIN_PADDING + SNAKE_PADDING,
                        y * GRID_CELL_SIZE + TOP_PADDING + SNAKE_PADDING,
                        GRID_CELL_SIZE - 2 * SNAKE_PADDING,
                        GRID_CELL_SIZE - 2 * SNAKE_PADDING, color);
          char text[4];
          int fontSize = Clamp(GRID_CELL_SIZE / 2.0f, 10, 64);
          sprintf(text, "%d", goalRequirementCounter);
          DrawText(text, x * GRID_CELL_SIZE + MAIN_PADDING + GRID_CELL_SIZE / 4,
                   y * GRID_CELL_SIZE + TOP_PADDING + GRID_CELL_SIZE / 2 -
                       fontSize / 2,
                   fontSize, GetColor(0x000000FF));
        }
        FillSnakeEdge(x, y, grid, color);
      }

      // Powerup - Length increase
      if (grid[gridPosTranslated] == CELL_POWERUP_LENGTH_INCREASE) {
        float cycleTime = frameCounter % ANIMATION_DURATION;
        float normalizedTime = cycleTime / ANIMATION_DURATION;
        float pingPongTime;
        if (normalizedTime < 0.5f) {
          pingPongTime = normalizedTime * 2.0f; // 0.0 to 1.0
        } else {
          pingPongTime = 2.0f - normalizedTime * 2.0f; // 1.0 to 0.0
        }

        float minScale = 0.4f;
        float maxScale = 0.65f;
        float scaleRange = maxScale - minScale;
        float easedProgress = EaseCircOut(pingPongTime, 0.0f, 1.0f, 1.0f);
        float scale = minScale + easedProgress * scaleRange;
        float width = GRID_CELL_SIZE * scale, height = GRID_CELL_SIZE * scale;

        DrawRectanglePro(
            (Rectangle){
                .x = x * GRID_CELL_SIZE + GRID_CELL_SIZE / 2.0f + MAIN_PADDING,
                .y = y * GRID_CELL_SIZE + GRID_CELL_SIZE / 2.0f + TOP_PADDING,
                .width = width,
                .height = height,
            },
            (Vector2){width / 2.0f, height / 2.0f},
            EaseLinearIn((float)frameCounter, 0.0f, 360.0f, ANIMATION_DURATION),
            GetColor(0xFF0000FF));
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

void HandleInputs(struct Snake *snake, bool *gameOver, bool *restart,
                  bool *roundWon, bool *muted, bool *paused) {
  if (*gameOver) {
    if (IsKeyDown(KEY_R)) {
      *restart = true;
    }
  }

  if (*roundWon) {
    if (IsKeyDown(KEY_ENTER)) {
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
