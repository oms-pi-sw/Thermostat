#ifndef _PERIPHERALS_H_
#define _PERIPHERALS_H_

#include <mbed.h>
#include "constants.h"

extern I2C* i2c;

void begin_i2c(void);

#endif // _PERIPHERALS_H_