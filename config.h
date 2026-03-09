#ifndef CONFIG_H
#define CONFIG_H

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define GRID_CELL_SIZE 10
#define DEBUG_MODE false
#define TIME_PER_TURN .05f
#define CELL_EMPTY 0
#define CELL_OBSTACLE -1
#define CELL_GOAL -2
#define MAIN_PADDING 20
#define TOP_PADDING 60
#define NO_COLUMNS (int)((WINDOW_WIDTH - 2 * MAIN_PADDING) / GRID_CELL_SIZE)
#define NO_ROWS                                                                \
  (int)((WINDOW_HEIGHT - TOP_PADDING - MAIN_PADDING) / GRID_CELL_SIZE)
#define GRID_SHIFT_THRESHOLD 5
#define GAME_TITLE "ourosnake"
#define INVINCIBLE true
#define THREADING_LEVEL 0
#define PAUSE_ENABLED true

#endif
