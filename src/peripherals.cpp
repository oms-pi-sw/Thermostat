#include "peripherals.hpp"

PeripheralsUtils::PeripheralsUtils() {
  this->uart = new Serial(USBTX, USBRX);
  this->i2c = new I2C(D_SDA, D_SCL);

  begin();
}

PeripheralsUtils::~PeripheralsUtils() {
  delete uart;
  delete i2c;
}

void PeripheralsUtils::begin(void) {
  begin_i2c();
  begin_uart();
}

bool PeripheralsUtils::start(void) {
  return (uart != NULL && i2c != NULL);
}


void PeripheralsUtils::begin_uart(void) {
  uart->baud(BAUD_RATE);
}

void PeripheralsUtils::begin_i2c(void) {
  i2c->frequency(I2C_FREQ);
}