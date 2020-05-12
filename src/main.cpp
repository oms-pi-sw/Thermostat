/* Example of Reading all the ADC pins PIN and display ot OLED display using
 Nucleo_F303K8. Displays voltage read from first 2 ADC lines on OLED display

  By Joseph Ellsworth CTO of A2WH
  Take a look at A2WH.com Producing Water from Air using Solar Energy
  March-2016 License: https://developer.mbed.org/handbook/MIT-Licence 
  Please contact us http://a2wh.com for help with custom design projects.

  
  Used to Drive:
   ebay part http://www.ebay.com/itm/152000005331?_trksid=p2057872.m2749.l2649&ssPageName=STRK%3AMEBIDX%3AIT
  0-96-I2C-IIC-SPI-Serial-White-OLED-LCD-LED-Display-Module-128X64
  The Datasheet.  https://www.adafruit.com/datasheets/SSD1306.pdf

  Unfortunately this part never turns on using the SSD1308 library.
  but did find that the https://developer.mbed.org/users/nkhorman/code/Adafruit_GFX/
  library works.  Unfortunately the Adafruit library doesn't include the scroll functionality.

  Using my I2C address scanner I found that it responds on Added 120 (x78) ,121 (x79)
  and when the part was unsoldered nothing responded on 120 or 121. According to
  to the page #19 (8.1.5) of the data sheet the I2C address should be 011110
  which seems to map correctly to dec=120 hex=79

 */

#include <mbed.h>
#include <math.h>
#include <string.h>
#include <Adafruit_SSD1306.h>
#include <BME280.h>

#define MAJOR                   0
#define MINOR                   1
#define REVISION                0

#define SCREEN_WIDTH            128
#define SCREEN_HEIGHT           64
#define SAMPLES_NUM             100
#define GRAPH_X                 (SCREEN_WIDTH - (SAMPLES_NUM + 1))
#define GRAPH_Y                 16
#define GRAPH_W                 (SAMPLES_NUM + 1)
#define GRAPH_H                 47
#define DATA_X                  10
#define DATA_Y                  0

#define D_SDA PB_9
#define D_SCL PB_8
I2C i2c(D_SDA, D_SCL);

DigitalOut statusLed(PC_13);

DigitalIn btn_plus(PA_0);
DigitalIn btn_minus(PA_1);
DigitalIn btn_menu(PA_6);
DigitalIn btn_ok(PA_4);
DigitalIn btn_up(PA_5);
DigitalIn btn_down(PA_7);

Ticker sample;
Timeout measure;

Adafruit_SSD1306_I2c *gOled2 = NULL;
BME280 *bme280 = NULL;

enum button {
  BTN_PLUS, BTN_MINS, BTN_MENU, BTN_OK, BTN_UP, BTN_DN
};
uint8_t btn_click = 0x00;
uint8_t btn_clicked = 0x00;

float temperatures[100], humidities[100], pressures[100];
volatile uint8_t samples = 0;

// Temperature, humidity, pressure, altitude
float temperature = 0., humidity = 0.;
float pressure = 0., altitude = 0.;
float t_max = 0, t_min = 0, h_max = 0, h_min = 0, p_max = 0, p_min = 0;

// Page
uint8_t page = 0;

volatile bool first = true;

volatile bool readyToSample = false;
volatile bool readyToMeasure = false;

float __max(const float x, const float y){
  return (x > y ? x : y);
}

void tickSample() {
  readyToSample = true;
}

void tickMeasure() {
  readyToMeasure = true;
}

void doSample() {
  if(samples < SAMPLES_NUM) {
    if(!first && !std::isnan(temperature) && !std::isinf(temperature) && !std::isnan(humidity) && !std::isinf(humidity) && !std::isnan(pressure) && !std::isinf(pressure)) {
      temperatures[samples] = temperature;
      humidities[samples] = humidity;
      pressures[samples] = pressure;
      ++samples;
    }
  } else {
    for(uint8_t i = 0; i < SAMPLES_NUM-1; i++) {
      temperatures[i] = temperatures[i + 1];
      humidities[i] = humidities[i + 1];
      pressures[i] = pressures[i + 1];
    }
    temperatures[SAMPLES_NUM-1] = temperature;
    humidities[SAMPLES_NUM-1] = humidity;
    pressures[SAMPLES_NUM-1] = pressure;
  }
}

void printTitle(const char *s, Adafruit_SSD1306_I2c *display, const bool newline, const bool print) {
  display->setTextCursor(0, 0);
  display->clearDisplay();
  display->setTextColor(BLACK, WHITE);
  uint8_t len = strlen(s);
  uint8_t x = 21 - len;
  uint8_t h = (uint8_t) (x / 2);
  for (uint8_t i = 0; i < h; i++) {
    display->printf(" ");
  }
  display->printf("%s", s);
  for (uint8_t i = 0; i < h; i++) {
    display->printf(" ");
  }
  if((h + h) < x) {
    display->printf(" ");
  }
  if(newline) {
    display->printf("\n\r");
  }
  display->setTextColor(WHITE, BLACK);
  if(print) {
    display->display();
  }
}

void readInput() {
  bool plus = btn_plus;
  bool minus = btn_minus;
  bool menu = btn_menu;
  bool ok = btn_ok;
  bool up = btn_up;
  bool down = btn_down;

  btn_clicked = btn_click;

  btn_click = (uint8_t) 0x00;

  btn_click |= ((plus & ((uint8_t) 0x01)) << BTN_PLUS);
  btn_click |= ((minus & ((uint8_t) 0x01)) << BTN_MINS);
  btn_click |= ((menu & ((uint8_t) 0x01)) << BTN_MENU);
  btn_click |= ((ok & ((uint8_t) 0x01)) << BTN_OK);
  btn_click |= ((up & ((uint8_t) 0x01)) << BTN_UP);
  btn_click |= ((down & ((uint8_t) 0x01)) << BTN_DN);
}

float __round(const float x) {
  return (float) roundf(x * 100.) / 100.;
}

void displayGraph(const uint8_t displayMode, const volatile float *sarray) {
  uint8_t i_stamp = samples;

  if (displayMode == 0)
    printTitle("TEMPERATURA (C)", gOled2, false, false);
  else if (displayMode == 1)
    printTitle("UMIDITA' (%)", gOled2, false, false);
  else if (displayMode == 2)
    printTitle("PRESSIONE (hPa)", gOled2, false, false);

  gOled2->drawFastVLine(GRAPH_X, GRAPH_Y, GRAPH_H, WHITE);
  gOled2->drawFastHLine(GRAPH_X, GRAPH_H + GRAPH_Y, GRAPH_W, WHITE);

  //int mean = 0;
  float mean = 0;
  float _max = __round(sarray[0]);
  float _min = __round(sarray[0]);

  for (uint8_t i = 0; i < i_stamp; i++) {
    if (_max < sarray[i]) {
      _max = __round(sarray[i]);
    }
    if (_min > sarray[i]) {
      _min = __round(sarray[i]);
    }
  }
  //mean = (int) (mean / i_stamp);
  mean = ((_max + _min) / 2);
  float max_delta = __max((abs(mean - _max)), (abs(mean - _min)));

  uint8_t x = 0;
  if(max_delta != 0)
    x = (uint8_t) ((GRAPH_Y + (GRAPH_H / 2)) + (((GRAPH_H / 2) * (- _max + mean)) / max_delta));
  else
    x = GRAPH_Y + (GRAPH_H / 2);

  gOled2->drawPixel(GRAPH_X - 1, x, WHITE);
  gOled2->setTextCursor(0,  x);
  gOled2->printf("%.1f", _max);

  if(max_delta != 0)
    x = (uint8_t) ((GRAPH_Y + (GRAPH_H / 2)) + (((GRAPH_H / 2) * (- _min + mean)) / max_delta));
  else
    x = GRAPH_Y + (GRAPH_H / 2);
  gOled2->drawPixel(GRAPH_X - 1, x, WHITE);
  gOled2->setTextCursor(0, x - 6);
  gOled2->printf("%.1f", _min);

  gOled2->drawPixel(GRAPH_X - 1, GRAPH_Y + (GRAPH_H / 2), WHITE);
  gOled2->setTextCursor(0,  GRAPH_Y + (GRAPH_H / 2) - 3);
  gOled2->printf("%.1f", mean);

  for (uint8_t i = 0; i < i_stamp; i++) {
    float _t = __round(sarray[i]);
    x = (uint8_t) ((GRAPH_Y + (GRAPH_H / 2)) + (((GRAPH_H / 2) * (- _t + mean)) / max_delta));
    gOled2->drawPixel(GRAPH_X + i + 1, x, WHITE);
  }

  gOled2->display();
}

void readSensors(void) {
  statusLed = !statusLed;
  bme280->trigger();
  while (bme280->busy())
  {
    wait_ms(10);
  }
  float _t1 = bme280->getTemperature();
  float _h1 = bme280->getHumidity();
  float _p1 = bme280->getPressure();

  if (!isnan(_t1)) {
    temperature = _t1;
    if (first) {
      t_min = temperature;
      t_max = temperature;
    } else {
      if (temperature < t_min) {
        t_min = temperature;
      }
      if (temperature > t_max) {
        t_max = temperature;
      }
    }
  }

  if (!isnan(_h1)) {
    humidity = _h1;
    if (first) {
      h_min = humidity;
      h_max = humidity;
    } else {
      if (humidity < h_min) {
        h_min = humidity;
      }
      if (humidity > h_max) {
        h_max = humidity;
      }
    }
  }

  if (!isnan(_p1)) {
    pressure = _p1;
    if (first) {
      p_min = pressure;
      p_max = pressure;
    } else {
      if (pressure < p_min) {
        p_min = pressure;
      }
      if (pressure > p_max) {
        p_max = pressure;
      }
    }
  }
}

void page0() {
  printTitle("MAIN", gOled2, true, false);
  gOled2->printf("T  : %.1f %cC\n\n", temperature, (char)247);
  gOled2->printf("HR : %.1f %%\n\n", humidity);
  gOled2->printf("P  : %.1f hPa\n", pressure);
  gOled2->display();
}

void page1() {
  printTitle("Max / Min", gOled2, true, false);

  gOled2->printf("T  max:  %.1f\n", t_max);
  gOled2->printf("T  min:  %.1f\n", t_min);

  gOled2->printf("HR max:  %.1f\n", h_max);
  gOled2->printf("HR min:  %.1f\n", h_min);

  gOled2->printf("P  max:  %.1f\n", p_max);
  gOled2->printf("P  min:  %.1f\n", p_min);

  gOled2->display();
}

void page2() {
  displayGraph(0, temperatures);
}

void page3() {
  displayGraph(1, humidities);
}

void page4() {
  displayGraph(2, pressures);
}

void page5() {
  printTitle("SAMPLES", gOled2, true, false);
  gOled2->printf("Samples # %d", samples);
  gOled2->display();
}

void refreshDisplay() {
  switch (page) {
    case 0:
      page0();
      break;
    case 1:
      page1();
      break;
    case 2:
      page2();
      break;
    case 3:
      page3();
      break;
    case 4:
      page4();
      break;
    case 5:
      page5();
      break;
    default:
      page = 0;
      refreshDisplay();
      break;
  }
}

void decodeInput() {
  readInput();

  if ((((btn_click >> BTN_DN) & ((uint8_t) 0x01)) == 1) && (((btn_clicked >> BTN_DN) & ((uint8_t) 0x01)) == 0)) {
    if (page < (uint8_t) 5) {
      ++page;
    }
  }
  if ((((btn_click >> BTN_UP) & ((uint8_t) 0x01)) == 1) && (((btn_clicked >> BTN_UP) & ((uint8_t) 0x01)) == 0)) {
    if (page > (uint8_t) 0) {
      --page;
    }
  }
}

void waitInput(const uint8_t cycles, const uint16_t ms, const bool refresh){
  for (uint8_t i = 0; i < cycles; i++) {
    decodeInput();
    if(refresh) refreshDisplay();
    wait_ms(ms);
  }
}

int main()
{
  first = true;
  wait_ms(500);
  i2c.frequency(100000);

  for (uint8_t i = 0; i < SAMPLES_NUM; i++) {
    temperatures[i] = 0.;
    humidities[i] = 0.;
    pressures[i] = 0.;
  }

  sample.attach(&tickSample, 2);
  measure.attach(&tickMeasure, 3);

  wait_ms(100);

  gOled2 = new Adafruit_SSD1306_I2c(i2c, PB_5, (0x3C << 1), 64, 128);

  bme280 = new BME280(&i2c, 0x76);

  gOled2->begin(SSD1306_SWITCHCAPVCC);

  //printTitle("TERMOSTATO", gOled2, true, true);
  //gOled2->printf("V %d.%d.%d\n\r", MAJOR, MINOR, REVISION);
  //gOled2->printf("Niccolo' Ferrari\n\r");
  //gOled2->printf("OMEGASOFTWARE\n\r");
  //gOled2->display();
  //wait_ms(1000);

  bme280->reset();
  wait_ms(2000);
  bme280->init();
  wait_ms(1000);

  while (1)
  {
    if(readyToMeasure) {
      readSensors();
      measure.attach(&tickMeasure, 3);
      readyToMeasure = false;
    }

    wait_ms(10);

    if(readyToSample) {
      doSample();
      readyToSample = false;
    }

    waitInput(1, 50, true);

    first = false;

  }
}