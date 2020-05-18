#ifndef _ESP_01_OMS_H_
#define _ESP_01_OMS_H_

#include <mbed.h>

#include "peripherals.hpp"

#define READ_TIMEOUT 0
#define READ_OK 1
#define READ_ERROR 2

#define BUFFER_CHUNK 1024

class ESP01_OMS {
 public:
  ESP01_OMS(Serial *uart) {
    this->muart = uart;

    NVIC_EnableIRQ(USART2_IRQn);

    using namespace std::placeholders;
    muart->attach(callback(this, &ESP01_OMS::i_rx), Serial::RxIrq);
    reset();
  }
  void reset(void) {
    reset_buffers();
    this->iReadF = 1;
    muart->printf("AT+RST\r\n");
    wait_ms(2000);
    flushSerialBuffer();
  }
  bool begin(void);

 private:
  Serial *muart;
  char rx_buffer[BUFFER_CHUNK], rx_line[BUFFER_CHUNK];
  volatile uint16_t rx_in;
  volatile uint16_t rx_out;
  Timeout countDown;
  volatile uint8_t iReadF;

  void flushSerialBuffer(void);
  void i_rx(void);
  void reset_buffers(void);
  char i_get_char(void);
  uint8_t i_read_OK(void);
  uint8_t i_print_read_OK(const char *str);
};

#endif  // _ESP_01_OMS_H_