#include "raylib.h"

#include "config.h"
#include "grid.h"
#include "obstacle.h"
#include "ui.h"

#include <stdio.h>

int main() {
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, GAME_TITLE);
  InitAudioDevice();
  int *grid = (int *)MemAlloc(NO_COLUMNS * NO_ROWS * sizeof(int));
  int currentRound = 0;
  int frameCounter = 0;
  Vector2 head, initialDirection;
  struct Snake snake;
  float time;
  bool gameOver, roundWon, restart, muted, paused;
  Sound stepWav = LoadSound("assets/audio/step.wav");
  Sound winWav = LoadSound("assets/audio/win.wav");
  Sound powerLengthUpWav = LoadSound("assets/audio/powerLengthUp.wav");
  Sound loseWav = LoadSound("assets/audio/lose.wav");
  Music bgWav = LoadMusicStream("assets/audio/bg.wav");
  Image mutedIcon = LoadImage("assets/icons/volume-mute-line.png");
  Texture2D mutedTexture = LoadTextureFromImage(mutedIcon);
  UnloadImage(mutedIcon);

  SetSoundVolume(stepWav, 0.2f);
  SetMusicVolume(bgWav, 0.5f);
  PlayMusicStream(bgWav);

  // Initial restart
  restart = true;
  muted = false;
  paused = false;
  printf("Number of rows [%d] columns [%d]\n", NO_ROWS, NO_COLUMNS);
  printf("Entering main game loop\n");

  SetTargetFPS(60);
  while (!WindowShouldClose()) {
    frameCounter++;
    UpdateMusicStream(bgWav);
    if (restart) {
      ClearGrid(grid);

      // TODO: find formula to ensure obstacles are not crowding
      // the grid / grid is not too empty
      // printf("Generating obstacles\n");
      GenerateInitialObstacles(grid, 5);
      // GenerateGoal(grid);
      GeneratePowerupLengthIncrease(grid);

      head =
          GetValidGridPosition(grid, CHECK_RADIUS, MAX_TRIES_PER_CHECK_RADIUS);

      initialDirection = directions[GetRandomValue(0, 3)];
      snake.head = head;
      snake.direction = initialDirection;
      snake.length = 5;
      time = 0;
      if (!roundWon)
        currentRound = 1;
      else
        currentRound++;
      gameOver = false;
      roundWon = false;
      restart = false;
    }

    if (!gameOver && !roundWon && !paused) {
      float dt = GetFrameTime();
      time += dt;
    }

    if (muted)
      PauseMusicStream(bgWav);
    else
      ResumeMusicStream(bgWav);

    HandleInputs(&snake, &gameOver, &restart, &roundWon, &muted, &paused);

    if (time > TIME_PER_TURN && !gameOver && !roundWon && !paused) {
      time -= TIME_PER_TURN;
      UpdateSnakePosition(grid, &snake);
      int currentHeadPosition =
          grid[(int)snake.head.y * NO_COLUMNS + (int)snake.head.x];

      if (currentHeadPosition == CELL_GOAL) {
        if (snake.length >= currentRound) {
          roundWon = true;
          PlaySoundWithMuteCheck(winWav, muted);
        } else {
          gameOver = true;
          PlaySoundWithMuteCheck(loseWav, muted);
        }
        goto draw;
      } else if (currentHeadPosition == CELL_POWERUP_LENGTH_INCREASE) {
        IncreaseSnakeLength(grid, &snake);
        GeneratePowerupLengthIncrease(grid);
        PlaySoundWithMuteCheck(powerLengthUpWav, muted);
      } else if (currentHeadPosition != CELL_EMPTY) {
        gameOver = true;
        PlaySoundWithMuteCheck(loseWav, muted);
        goto draw;
      }

      UpdateCellLives(grid, snake);
      PlaySoundWithMuteCheck(stepWav, muted);
    }
  draw:
    if (!restart) {
      BeginDrawing();
      ClearBackground(GetColor(0x202020FF));

      DrawObjects(grid, frameCounter, snake.length >= currentRound);
      if (DEBUG_MODE) {
        DrawDebugCellValues(grid);
      }
      DrawGrid2D();
      DrawGameHeader(&currentRound);
      if (roundWon) {
        DrawEndRoundBox("Round Won", "Press enter to continue", 64, 12, GREEN);
      }
      if (gameOver) {
        DrawEndRoundBox("Game Over", "Press R to restart", 64, 12, RED);
      }
      if (paused) {
        DrawEndRoundBox("Paused", "Press P to resume", 64, 12, BLUE);
      }
      if (muted) {
        DrawTexture(mutedTexture, WINDOW_WIDTH - 30, 30, RED);
      }
      EndDrawing();
    }
  }

  CloseAudioDevice();
  UnloadTexture(mutedTexture);
  UnloadMusicStream(bgWav);
  CloseWindow();
  MemFree(grid);
  return 0;
}
