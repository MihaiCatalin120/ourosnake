#ifndef UNITY_BUILD
#define UNITY_BUILD
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

#endif
