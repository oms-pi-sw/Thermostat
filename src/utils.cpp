#include "utils.hpp"

uint32_t Utils::__strlen(const char* s) {
  uint32_t len = 0;
  while (s[len] != 0x0) {
    ++len;
  }
  return (len);
}

float Utils::__max(const float x, const float y) { return (x > y ? x : y); }

float Utils::__round(const float x) { return (float)roundf(x * 100.) / 100.; }

const char* Utils::__ftos(char* s, const float f) {
  uint16_t v = (uint16_t)(f * 10);
  char is[10];
  __itoa(v, is, 10);
  bool lookf = true;
  uint8_t k = 0, l = 0;
  while (is[k] != 0x0) {
    if (lookf && is[k + 1] == 0x0) {
      s[l++] = '.';
      lookf = false;
    } else {
      s[l++] = is[k++];
    }
  }
  s[l] = (char)0x0;
  return s;
}

float Utils::__mean(const float* f, uint8_t len) {
  float r = 0.;
  for (uint8_t i = 0; i < len; i++) r += f[i];
  r /= (float)len;
  return (float)r;
}

float Utils::__round2d(const float f) {
  return (float)((round((f * 100.))) / (100.));
}