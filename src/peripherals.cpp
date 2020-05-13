#include "peripherals.hpp"

I2C* i2c = NULL;
Serial* uart = NULL;

void begin_uart(void) {
  uart = new Serial(USBTX, USBRX);
  uart->baud(BAUD_RATE);
}

void begin_i2c(void) {
  i2c = new I2C(D_SDA, D_SCL);
  i2c->frequency(I2C_FREQ);
}