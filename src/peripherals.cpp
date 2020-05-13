#include "peripherals.hpp"

I2C* i2c = NULL;

void begin_i2c(void) {
  i2c = new I2C(D_SDA, D_SCL);
  i2c->frequency(400000);
}