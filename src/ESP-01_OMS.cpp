#include "ESP-01_OMS.hpp"

void ESP01_OMS::i_rx(void) {
  while ((muart->readable()) && (((rx_in + 1) % BUFFER_CHUNK) != rx_out)) {
    rx_buffer[rx_in] = uart->getc();
    rx_in = (rx_in + 1) % BUFFER_CHUNK;
  }
  return;
}

void ESP01_OMS::flushSerialBuffer(void) {
  while (muart->readable()) {
    muart->getc();
  }
}

void ESP01_OMS::reset_buffers(void) {
  memset(rx_buffer, 0, BUFFER_CHUNK * sizeof(char));
  memset(rx_line, 0, BUFFER_CHUNK * sizeof(char));

  rx_in = 0;
  rx_out = 0;
}

char ESP01_OMS::i_get_char(void) {
  char c = (char)0;
  if (rx_in == rx_out) return (char)0;
  NVIC_DisableIRQ(USART2_IRQn);
  c = rx_buffer[rx_out];
  rx_out++;
  rx_out %= BUFFER_CHUNK;
  NVIC_EnableIRQ(USART2_IRQn);
  return c;
}

bool ESP01_OMS::begin(void) {
  return (i_print_read_OK(
              "AT+CWJAP=\"NETGEAR_7000\",\"SpegniQuellaLuce\"\r\n") == READ_OK);
}

uint8_t ESP01_OMS::i_read_OK(void) {
  char l_buffer[BUFFER_CHUNK], c = 0;
  uint32_t i = 0;
  memset(l_buffer, 0, BUFFER_CHUNK);
  iReadF = 1;
  while (iReadF) {
    c = 0;
    while (c == 0) c = i_get_char();
    if (c) {
      l_buffer[i] = c;
      i++;
      if (c == '\n') {
        if (strstr(l_buffer, "OK")) return READ_OK;
        if (strstr(l_buffer, "ERROR")) return READ_ERROR;
        i = 0;
        memset(l_buffer, 0, BUFFER_CHUNK);
      }
    }
  }
  return READ_TIMEOUT;
}

uint8_t ESP01_OMS::i_print_read_OK(const char *str) {
  uint8_t res;
  do {
    muart->printf(str);
    res = i_read_OK();
  } while (res == READ_TIMEOUT);
  return res;
}