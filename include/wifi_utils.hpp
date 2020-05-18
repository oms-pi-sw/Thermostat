#ifndef _WIFI_UTILS_H_
#define _WIFI_UTILS_H_

#include <mbed.h>

#include "ESP-01_OMS.hpp"

extern ESP01_OMS *wifi;

bool begin_wifi(void);

#endif // _WIFI_UTILS_H_