#ifndef _WIFI_UTILS_H_
#define _WIFI_UTILS_H_

#include <mbed.h>

#include "ESP-01_OMS.hpp"

#define SCHEDULE_MAX_SIZE 512

enum queryStates {
  STATE_FLUSH,
  STATE_START,
  STATE_LINKED,
  STATE_SEND,
  STATE_HTTP,
  STATE_READ
};

extern ESP01_OMS *wifi;

extern volatile uint8_t queryState;
extern volatile bool querySucceded;
extern volatile bool queryExecute;

extern char schedule[];

bool begin_wifi(void);
bool begin_schedule(void);

void start_sync(void);

bool query_flush(void);
bool query_start(void);
bool query_linked(void);
bool query_send(void);
bool query_http(void);
bool query_read(void);
bool query_fsm(void);

#endif  // _WIFI_UTILS_H_