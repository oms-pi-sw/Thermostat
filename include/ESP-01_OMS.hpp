#ifndef _ESP_01_OMS_H_
#define _ESP_01_OMS_H_

#include <mbed.h>

#include "utils.hpp"

#define READ_TIMEOUT 0
#define READ_OK 1
#define READ_ERROR 2

#define BUFFER_CHUNK 2048

class ESP01_OMS {
 public:
  static const char http_query[];

  ESP01_OMS(Serial *uart) {
    this->muart = uart;

    NVIC_EnableIRQ(USART2_IRQn);

    muart->attach(callback(this, &ESP01_OMS::i_rx), Serial::RxIrq);
    reset();
  }
  void reset(void) {
    for (uint8_t i = 0; i < 3; i++) flushSerialBuffer();
    reset_buffers();
    wait_ms(100);
    connected = false;
    this->iReadF = 1;
    print("AT+RST\r\n");
    wait_ms(3000);
    reset_buffers();
    flushSerialBuffer();
  }
  bool begin(void);
  bool is_connected(void) { return this->connected; }
  char i_get_char(void);
  Serial *get_serial_chn() { return this->muart; }
  bool query_db(void);
  bool send_query(void);
  void flushBufferAndSerial(void);
  bool is_pending_data(void) { return (rx_in != rx_out); }
  uint32_t print(const char *string);

 private:
  Serial *muart;
  char rx_buffer[BUFFER_CHUNK], rx_line[BUFFER_CHUNK];
  volatile uint16_t rx_in;
  volatile uint16_t rx_out;
  Timeout countDown;
  volatile uint8_t iReadF;
  volatile bool connected;

  void flushSerialBuffer(void);
  void i_rx(void);
  void reset_buffers(void);
  uint8_t i_read_OK(void);
  uint8_t i_print_read_OK(const char *str);
};

#endif  // _ESP_01_OMS_H_