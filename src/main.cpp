#include <mbed.h>

#include "version.h"
#include "constants.h"

#include "BME280.h"
#include "ESP-01_OMS.hpp"
#include "SSD1306_OMS_lite.hpp"

#include "utils.hpp"
#include "peripherals.hpp"
#include "sensors_utils.hpp"
#include "display_utils.hpp"
#include "input_utils.hpp"
//#include "wifi_utils.hpp"

volatile bool first = 0;

PeripheralsUtils *p = nullptr;
SensorsUtils *s = nullptr;
DisplayUtils *d = nullptr;
InputUtils *i = nullptr;

bool p_start = false;
bool s_start = false;
bool d_start = false;
bool i_start = false;
bool w_start = false;

int main() {
  first = true;
  wait_ms(500);

  // Begin Peripherals
  p = new PeripheralsUtils();
  // Start Peripherals
  p_start = p->start();

  // Begin Sensors
  s = new SensorsUtils(p);
  // Begin Display
  d = new DisplayUtils(p, s);
  // Begin Input
  i = new InputUtils(d);

  // Start Display
  d_start = d->start();

  // Check Peripherals
  if (p_start) {
    d->print("Init I2C...   Done.\n");
    d->print("Init UART...  Done.\n");
  }

  // Check Display
  if (d_start) {
    d->print("Init DSP...   Done.\n");
  }
  d->flush();

  d->print("Init Input... ");
  d->flush();
  // Start Input
  i_start = i->start();
  // Check Input
  if (i_start)
    d->print("Done.\n");
  else
    d->print("ERROR.\n");
  d->flush();

  d->print("Init BME...   ");
  d->flush();
  // Start Sensors
  s_start = s->start();
  // Check Sensors
  if (s_start)
    d->print("Done.");
  else
    d->print("Error.");
  d->flush();

  /*gOled2->printString("Init WIFI...  ");
  gOled2->display();
  if (begin_wifi()) {
    gOled2->printString("Done.");
  } else {
    gOled2->printString("ERROR.");
  }
  gOled2->display();
  begin_schedule();

  wait_ms(1000);
  start_sync();*/

  while (1) {
    s->cycle();
    wait_ms(1);
    d->cycle();
    wait_ms(1);
    i->cycle();
    wait_ms(1);

    //query_fsm();
    //sample_fsm();

    first = false;
  }
}