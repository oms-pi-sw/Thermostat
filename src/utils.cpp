#include "utils.hpp"

uint8_t __strlen(const char *s) {
  uint8_t len = 0;
  while (s[len] != 0x0) {
    ++len;
  }
  return (len);
}

float __max(const float x, const float y) { return (x > y ? x : y); }

float __round(const float x) { return (float)roundf(x * 100.) / 100.; }