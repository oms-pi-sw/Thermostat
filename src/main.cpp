#include <mbed.h>

#include "constants.h"

#ifndef USE_OMS_SSD1306_DRIVER
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#else
#include "SSD1306_OMS.hpp"
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
  begin_display();
  begin_sensors();

  sample.attach(&tickSample, 1);
  measure.attach(&tickMeasure, 3);

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

    if(btn_pressed[BTN_DN]) {
      btn_pressed[BTN_DN] = false;
      if(page < 5) ++page;
    }
    if(btn_pressed[BTN_UP]) {
      btn_pressed[BTN_UP] = false;
      if(page > 0) --page;
    }

    first = false;
  }
}