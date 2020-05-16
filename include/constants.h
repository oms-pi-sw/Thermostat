#ifndef _COSTANTS_H_
#define _COSTANTS_H_

#include "version.h"

/*
 * SETTINGS SECTION
*/

#define USE_OMS_SSD1306_DRIVER
#ifdef USE_OMS_SSD1306_DRIVER
#define USE_OMS_SSD1306_DRIVER_LITE
#endif

//#define USE_OMS_BME280_DRIVER

//#define USE_INPUT_INTERRUPTS

//#define USE_FULL_FONT

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

#ifndef USE_OMS_SSD1306_DRIVER
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#else
#ifndef USE_OMS_SSD1306_DRIVER_LITE
#include "SSD1306_OMS.hpp"
#else
#include "SSD1306_OMS_lite.hpp"
#endif
#endif

#ifndef USE_OMS_SSD1306_DRIVER
#define DISPLAY_TYPE Adafruit_SSD1306_I2c
#else
#ifndef USE_OMS_SSD1306_DRIVER_LITE
#define DISPLAY_TYPE SSD1306_OMS
#else
#define DISPLAY_TYPE SSD1306_OMS_lite
#endif
#endif

extern volatile bool first;

#endif // _COSTANTS_H_