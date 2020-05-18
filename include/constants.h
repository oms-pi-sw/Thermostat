#ifndef _COSTANTS_H_
#define _COSTANTS_H_

#include "version.h"

/*
 * SETTINGS SECTION
*/

//#define USE_INPUT_INTERRUPTS

#define USE_MEMSET

/*
 * END SETTINGS SECTION
*/

#ifndef USE_INPUT_INTERRUPTS
#define WAIT_MS                 10
#define CYCLES                  1
#else
#define WAIT_MS                 100
#define CYCLES                  1
#endif

#define SCREEN_WIDTH            128
#define SCREEN_HEIGHT           64

#define SAMPLES_NUM             100
#define GRAPH_X                 (SCREEN_WIDTH - (SAMPLES_NUM + 1))
#define GRAPH_Y                 16
#define GRAPH_W                 (SAMPLES_NUM + 1)
#define GRAPH_H                 47
#define DATA_X                  10
#define DATA_Y                  0

#define BUTTON_NUMBER           6

#define D_SDA                   PB_9
#define D_SCL                   PB_8

#define USBTX                   PA_2
#define USBRX                   PA_3

#define BAUD_RATE               9600
#define I2C_FREQ                1000000

#define PAGE_MAIN               0
#define PAGE_MAXMIN             1
#define PAGE_SENSORS            2
#define PAGE_TEMPERATURE        3
#define PAGE_HUMIDITY           4
#define PAGE_PRESSURE           5

#define MENU_NO                 0
#define MENU_MAIN               1
#define MENU_WIFI               2
#define MENU_DATETIME           3
#define MENU_EXIT               4

#define MENU_S_TEMPERATURE      0
#define MENU_S_WIFI             1
#define MENU_S_DATETIME         2
#define MENU_S_EXIT             3

#include "SSD1306_OMS_lite.hpp"


#define DISPLAY_TYPE SSD1306_OMS_lite

extern volatile bool first;

#endif // _COSTANTS_H_