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
  if (begin_wifi()) {
    gOled2->printString("Init WIFI...  Done.");
    gOled2->display();
  }

  sample.attach(&tickSample, 1);
  measure.attach(&tickMeasure, 3);

/*
    gOled2->clearDisplay();
    gOled2->setTextCursor(0, 0);
    uart->printf("AT+RST\r\n");
    gOled2->printString("RESET\n");
    gOled2->display();
    wait_ms(2000);
    reset_buffers();
    wait_ms(100);

    gOled2->clearDisplay();
    gOled2->setTextCursor(0, 0);
    uart->printf("AT+CWMODE?\r\n");
    wait_ms(100);
    char c = 0;
    while ((c = i_get_char()) != 0) {
      gOled2->writeChar(c);
      gOled2->display();
    };
    wait_ms(1000);

    gOled2->clearDisplay();
    gOled2->setTextCursor(0, 0);
    uart->printf("AT+CWJAP=\"NETGEAR_7000\",\"SpegniQuellaLuce\"\r\n");
    wait_ms(5000);
    c = 0;
    while ((c = i_get_char()) != 0) {
      gOled2->writeChar(c);
      gOled2->display();
    };
    wait_ms(2000);

    gOled2->clearDisplay();
    gOled2->setTextCursor(0, 0);
    uart->printf("AT+CIFSR\r\n");
    wait_ms(500);
    c = 0;
    while ((c = i_get_char()) != 0) {
      gOled2->writeChar(c);
      gOled2->display();
    };
    wait_ms(500);

    gOled2->clearDisplay();
    gOled2->setTextCursor(0, 0);
    uart->printf("AT+CIPSTART=\"TCP\",\"minegrado.ovh\",80\r\n");
    wait_ms(1000);
    c = 0;
    while ((c = i_get_char()) != 0) {
      gOled2->writeChar(c);
      gOled2->display();
    };
    wait_ms(500);

    gOled2->clearDisplay();
    gOled2->setTextCursor(0, 0);
    char http_req[] = "GET /test HTTP/1.1\r\nHost: minegrado.ovh:80\r\n\r\n";
    uart->printf("AT+CIPSEND=%d\r\n", __strlen(http_req));
    wait_ms(100);
    uart->printf(http_req);
    wait_ms(5000);
    c = 0;
    while ((c = i_get_char()) != 0) {
      gOled2->writeChar(c);
      gOled2->display();
    };
    wait_ms(50000);

    gOled2->clearDisplay();
    gOled2->setTextCursor(0, 0);
    gOled2->printString("CONNECTED\n");
    gOled2->display();
*/

    wait_ms(1000);

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

    first = false;
  }
}