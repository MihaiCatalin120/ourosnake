#ifndef CONFIG_H
#define CONFIG_H

// WARN: If NO_COLUMNS or NO_ROWS have floating points the game will not work
// properly
#define WINDOW_WIDTH 840
#define WINDOW_HEIGHT 680
#define GRID_CELL_SIZE 50
#define NO_COLUMNS (int)((WINDOW_WIDTH - 2 * MAIN_PADDING) / GRID_CELL_SIZE)
#define NO_ROWS                                                                \
  (int)((WINDOW_HEIGHT - TOP_PADDING - MAIN_PADDING) / GRID_CELL_SIZE)

#define DEBUG_MODE false
#define TIME_PER_TURN .4f // .4 matches 150bpm of bg music

// WARN: Special cells will be affected if set to positive numbers
#define CELL_EMPTY 0
#define CELL_OBSTACLE -1
#define CELL_GOAL 1
#define CELL_POWERUP_LENGTH_INCREASE -2

// INFO: Spacing
#define MAIN_PADDING 20
#define TOP_PADDING 60
#define SNAKE_PADDING 5

#define GAME_TITLE "ourosnake"
#define PAUSE_ENABLED true
#define CHECK_RADIUS 3
#define MAX_TRIES_PER_CHECK_RADIUS 10
#define ANIMATION_DURATION 180 // 60 fps * 3 seconds

#endif
