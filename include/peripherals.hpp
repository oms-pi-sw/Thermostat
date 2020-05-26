#ifndef _PERIPHERALS_H_
#define _PERIPHERALS_H_

#include <mbed.h>

#include "constants.h"

class PeripheralsUtils {
 public:
  PeripheralsUtils();
  virtual ~PeripheralsUtils();
  bool start(void);
  I2C* getI2C(void) const { return this->i2c; };
  Serial* getUART(void) const { return this->uart; };

 protected:
 private:
  void begin(void);
  void begin_uart(void);
  void begin_i2c(void);

  I2C* i2c;
  Serial* uart;
};

#endif  // _PERIPHERALS_H_