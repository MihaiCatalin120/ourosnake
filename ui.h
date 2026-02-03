#include "raylib.h"
#include "snake.h"

#ifndef UI_H
#define UI_H

void DrawGrid2D();                   // Draws the grid lines
void DrawGameHeader(int *round);     // Draw the elements from the game header
void DrawDebugCellValues(int *grid); // Shows the grid values
void DrawObjects(
    int *grid); // Draws the filling objects (snake, obstacles, goal)
void DrawEndRoundBox(
    const char mainText[], const char secondaryText[], int mainFontSize,
    int secondaryFontSize,
    Color mainTextColor); // Draws a box in the center of the screen with 2
                          // lines of text provided as input
void HandleInputs(int *grid, struct Snake *snake, bool *gameOver,
                  int *currentRound, bool *restart, bool *roundWon, bool *muted,
                  bool *paused);

#endif
