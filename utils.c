#include "utils.h"
#include <stdio.h>

Vector2 directions[4] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};

void PlaySoundWithMuteCheck(Sound sound, bool muted) {
  if (!muted)
    PlaySound(sound);
}

bool IsValueInArray(int value, int array[], int length) {
  for (size_t i = 0; i < length; i++) {
    if (value == array[i])
      return true;
  }

  return false;
}
