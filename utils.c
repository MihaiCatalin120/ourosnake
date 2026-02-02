#include "utils.h"
#include <stdio.h>

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
