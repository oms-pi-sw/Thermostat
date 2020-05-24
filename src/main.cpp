#include <mbed.h>

#include "BME280.h"
#include "ESP-01_OMS.hpp"
#include "SSD1306_OMS_lite.hpp"
#include "constants.h"
#include "display_utils.hpp"
#include "input_utils.hpp"
#include "peripherals.hpp"
#include "sensors_utils.hpp"
#include "utils.hpp"
#include "version.h"
#include "wifi_utils.hpp"

volatile bool first = 0;

Ticker sample;
Timeout measure;

int main() {
  first = true;
  wait_ms(500);

  begin_i2c();
  begin_display();

  gOled2->printString("Init I2C...   Done.\n");
  gOled2->printString("Init DSP...   Done.\n");

  gOled2->display();

  if (begin_input()) {
    gOled2->printString("Init Input... Done.\n");
    gOled2->display();
  }
  if (begin_uart()) {
    gOled2->printString("Init UART...  Done.\n");
    gOled2->display();
  }
  if (begin_sensors()) {
    gOled2->printString("Init BME...   Done.\n");
    gOled2->display();
  }
  gOled2->printString("Init WIFI...  ");
  gOled2->display();
  if (begin_wifi()) {
    gOled2->printString("Done.");
  } else {
    gOled2->printString("ERROR.");
  }
  gOled2->display();
  begin_schedule();

  sample.attach(&tickSample, SAMPLE_T);
  measure.attach(&tickMeasure, 5);

  wait_ms(1000);
  start_sync();

  while (1) {
    if (readyToMeasure) {
      readSensors();
      measure.attach(&tickMeasure, MEASURE_T);
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
      if ((!menu && page < 5) || (menu && page < 3)) ++page;
    }
    if (btn_pressed[BTN_UP]) {
      btn_pressed[BTN_UP] = false;
      if (page > 0) --page;
    }
    if (btn_pressed[BTN_MENU]) {
      btn_pressed[BTN_MENU] = false;
      page = MENU_S_TEMPERATURE;
      menu = MENU_MAIN;
    }
    if (btn_pressed[BTN_OK]) {
      btn_pressed[BTN_OK] = false;
      if (menu == MENU_MAIN && page == MENU_S_EXIT) {
        page = PAGE_MAIN;
        menu = MENU_NO;
      }
    }

    query_fsm();

    first = false;
  }
}