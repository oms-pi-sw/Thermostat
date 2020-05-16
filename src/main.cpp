#include <mbed.h>

#include "constants.h"

#ifndef USE_OMS_SSD1306_DRIVER
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#else
#ifndef USE_OMS_SSD1306_DRIVER_LITE
#include "SSD1306_OMS.hpp"
#else
#include "SSD1306_OMS_lite.hpp"
#endif
#endif

#include "display_utils.hpp"
#include "input_utils.hpp"
#include "peripherals.hpp"
#include "sensors_utils.hpp"
#include "utils.hpp"
#include "version.h"

volatile bool first = 0;

Ticker sample;
Timeout measure;

int main() {
  first = true;
  wait_ms(500);

  begin_input();
  begin_i2c();
  begin_uart();
  begin_display();
  begin_sensors();

  sample.attach(&tickSample, 1);
  measure.attach(&tickMeasure, 3);

  gOled2->clearDisplay();
  gOled2->setTextCursor(0, 0);
  char buf[50];
  memset(buf, 0, 50 * sizeof(char));
  uint8_t k = 0;
  uart->printf("AT+RST\r\n");
  gOled2->printString("RESET\n");
  gOled2->display();
  wait_ms(2000);
  reset_buffers();
  wait_ms(100);
  uart->printf("AT+CWMODE?\r\n");
  gOled2->display();
  wait_ms(100);
  char c = 0;
  while ((c = i_get_char()) != 0) {
    gOled2->writeChar(c);
    gOled2->display();
    ++k;
  };
  gOled2->printString("\nBYE\n");
  gOled2->printInt(k);
  gOled2->display();

  wait(20);

  while (1) {
    if (readyToMeasure) {
      readSensors();
      measure.attach(&tickMeasure, 3);
      readyToMeasure = false;
    }
    wait_ms(10);

    if (readyToSample) {
      doSample();
      readyToSample = false;
    }

    waitInput(CYCLES, WAIT_MS, true);

    if (btn_pressed[BTN_DN]) {
      btn_pressed[BTN_DN] = false;
      if (page < 5) ++page;
    }
    if (btn_pressed[BTN_UP]) {
      btn_pressed[BTN_UP] = false;
      if (page > 0) --page;
    }

    first = false;
  }
}