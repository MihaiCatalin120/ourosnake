#include "raylib.h"
#ifndef UTILS_H
#define UTILS_H

struct Snake {
  Vector2 head;
  Vector2 direction;
  int length;
};

struct ObstacleGenerator {
  Vector2 startPosition;
  Vector2 moves[10]; // TODO: support dynamic moves max size
};

// Up, Right, Down, Left
extern Vector2 directions[4];

void PlaySoundWithMuteCheck(
    Sound sound, bool muted); // Wrapper for PlaySound with a muted check on top
bool IsValueInArray(
    int value, int array[],
    int length); // Check if a value is in given array (int only)

#endif
