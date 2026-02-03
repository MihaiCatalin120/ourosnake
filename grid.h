#include "snake.h"

#ifndef GRID_H
#define GRID_H

void UpdateCellLives(
    int *grid,
    struct Snake snake);   // Performs a iteration on the grid, updating all the
                           // appropiate cell positions
void ClearGrid(int *grid); // Sets all the grid cells to empty
void GenerateGoal(int *grid); // Generate a goal cell on the grid
                              //
#endif
