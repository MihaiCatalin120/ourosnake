#include "config.h"
#include "grid.h"
#include "obstacle.h"
#include "snake.h"
#include "ui.h"
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

    if (time > TIME_PER_TURN && !gameOver && !roundWon && !paused) {
      time -= TIME_PER_TURN;
      UpdateSnakePosition(grid, &snake);

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

      UpdateCellLives(grid, snake);
      PlaySoundWithMuteCheck(stepWav, muted);
    }
  draw:
    if (!restart) {
      BeginDrawing();
      ClearBackground(GetColor(0x202020FF));

      DrawObjects(grid);
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
  CloseWindow();
  MemFree(grid);
  return 0;
}
