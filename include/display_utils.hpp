#ifndef _DISPLAY_UTILS_H_
#define _DISPLAY_UTILS_H_

#include <mbed.h>
#include "constants.h"

#include "sensors_utils.hpp"


#include "SSD1306_OMS_lite.hpp"

#include "utils.hpp"

extern DISPLAY_TYPE *gOled2;

// Page
extern uint8_t page;
// Menu
extern uint8_t menu;

bool begin_display(void);
void printTitle(const char *s);
void displayGraph(const volatile float *sarray);
void refreshDisplay(void);
void displaySplash(void);

#endif // _DISPLAY_UTILS_H_