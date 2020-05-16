#ifndef _DISPLAY_UTILS_H_
#define _DISPLAY_UTILS_H_

#include <mbed.h>
#include "constants.h"

#include "sensors_utils.hpp"

#ifndef USE_OMS_SSD1306_DRIVER
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#else
#ifndef USE_OMS_SSD1306_DRIVER_LITE
#include "SSD1306_OMS.hpp"
#else
#include "SSD1306_OMS_lite.hpp"
#endif
#endif

#include "utils.hpp"

extern DISPLAY_TYPE *gOled2;

// Page
extern uint8_t page;

void begin_display(void);
#ifndef USE_OMS_SSD1306_DRIVER_LITE
void printTitle(const char *s, DISPLAY_TYPE *display, const bool newline, const bool print);
#else
void printTitle(const char *s);
#endif
void displayGraph(const volatile float *sarray);
void page0(void);
void page1(void);
void page2(void);
void page3(void);
void page4(void);
void page5(void);
void refreshDisplay(void);
void displaySplash(void);

#endif // _DISPLAY_UTILS_H_