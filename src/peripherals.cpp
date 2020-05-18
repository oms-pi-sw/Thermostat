#include "peripherals.hpp"

I2C* i2c = NULL;
Serial* uart = NULL;

bool begin_uart(void) {
  uart = new Serial(USBTX, USBRX);
  uart->baud(BAUD_RATE);

  return (uart != NULL);
}

bool begin_i2c(void) {
  i2c = new I2C(D_SDA, D_SCL);
  i2c->frequency(I2C_FREQ);

  return (i2c != NULL);
}