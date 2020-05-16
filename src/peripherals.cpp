#include "peripherals.hpp"

I2C* i2c = NULL;
Serial* uart = NULL;

char rx_buffer[BUFFER_CHUNK], rx_line[BUFFER_CHUNK];

volatile uint16_t rx_in = 0;
volatile uint16_t rx_out = 0;

Timeout countDown;

volatile uint8_t iReadF = 1;

// INTERRUPT
void i_rx(void) {
  while ((uart->readable()) && (((rx_in + 1) % BUFFER_CHUNK) != rx_out)) {
    rx_buffer[rx_in] = uart->getc();
    rx_in = (rx_in + 1) % BUFFER_CHUNK;
  }
  return;
}

void flushSerialBuffer(void) {
  while (uart->readable()) {
    uart->getc();
  }
}

void begin_uart(void) {
  memset(rx_buffer, 0, BUFFER_CHUNK * sizeof(char));
  memset(rx_line, 0, BUFFER_CHUNK * sizeof(char));

  uart = new Serial(USBTX, USBRX);
  uart->baud(BAUD_RATE);

  flushSerialBuffer();

  NVIC_EnableIRQ(USART2_IRQn);

  uart->attach(&i_rx, Serial::RxIrq);
}

void reset_buffers(void) {
  memset(rx_buffer, 0, BUFFER_CHUNK * sizeof(char));
  memset(rx_line, 0, BUFFER_CHUNK * sizeof(char));

  rx_in = 0;
  rx_out = 0;
}

char i_get_char(void) {
  char c = (char)0;
  if (rx_in == rx_out) return (char)0;
  NVIC_DisableIRQ(USART2_IRQn);
  c = rx_buffer[rx_out];
  rx_out++;
  rx_out %= BUFFER_CHUNK;
  NVIC_EnableIRQ(USART2_IRQn);
  return c;
}

void begin_i2c(void) {
  i2c = new I2C(D_SDA, D_SCL);
  i2c->frequency(I2C_FREQ);
}