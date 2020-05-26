#ifndef _UTILS_H_
#define _UTILS_H_

#include <mbed.h>

#include "constants.h"

class Utils {
 public:
  static uint32_t __strlen(const char *s);
  static float __max(const float x, const float y);
  static float __round(const float x);
  static const char* __ftos(char *s, const float f);
  static float __mean(const float* f, uint8_t len);
  static float __round2d(const float f);
};

#endif  // _UTILS_H_