#ifndef UNITY_BUILD
#define UNITY_BUILD
#include "raylib.h"

// Up, Right, Down, Left
Vector2 directions[4] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};

void PlaySoundWithMuteCheck(
    Sound sound, bool muted); // Wrapper for PlaySound with a muted check on top
bool IsValueInArray(
    int value, int array[],
    int length); // Check if a value is in given array (int only)
#endif
