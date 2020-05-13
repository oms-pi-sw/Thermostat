/* Example of Reading all the ADC pins PIN and display ot OLED display using
 Nucleo_F303K8. Displays voltage read from first 2 ADC lines on OLED display

  By Joseph Ellsworth CTO of A2WH
  Take a look at A2WH.com Producing Water from Air using Solar Energy
  March-2016 License: https://developer.mbed.org/handbook/MIT-Licence
  Please contact us http://a2wh.com for help with custom design projects.


  Used to Drive:
   ebay part
 http://www.ebay.com/itm/152000005331?_trksid=p2057872.m2749.l2649&ssPageName=STRK%3AMEBIDX%3AIT
  0-96-I2C-IIC-SPI-Serial-White-OLED-LCD-LED-Display-Module-128X64
  The Datasheet.  https://www.adafruit.com/datasheets/SSD1306.pdf

  Unfortunately this part never turns on using the SSD1308 library.
  but did find that the
 https://developer.mbed.org/users/nkhorman/code/Adafruit_GFX/ library works.
 Unfortunately the Adafruit library doesn't include the scroll functionality.

  Using my I2C address scanner I found that it responds on Added 120 (x78) ,121
 (x79) and when the part was unsoldered nothing responded on 120 or 121.
 According to to the page #19 (8.1.5) of the data sheet the I2C address should
 be 011110 which seems to map correctly to dec=120 hex=79

 */


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