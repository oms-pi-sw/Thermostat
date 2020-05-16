#ifndef _PERIPHERALS_H_
#define _PERIPHERALS_H_

#include <mbed.h>
#include "constants.h"
#define BUFFER_CHUNK 1024

extern I2C* i2c;
extern Serial* uart;

void begin_uart(void);
void begin_i2c(void);

void reset_buffers(void);
char i_get_char(void);

#endif  // _PERIPHERALS_H_