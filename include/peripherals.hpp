#ifndef _PERIPHERALS_H_
#define _PERIPHERALS_H_

#include <mbed.h>

#include "constants.h"

extern I2C* i2c;
extern Serial* uart;

bool begin_uart(void);
bool begin_i2c(void);

void reset_buffers(void);
char i_get_char(void);

#endif  // _PERIPHERALS_H_