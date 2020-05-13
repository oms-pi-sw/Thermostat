#include "display_utils.hpp"

DISPLAY_TYPE *gOled2 = NULL;

// Page
uint8_t page = 0;

void begin_display(void) {
  page = 0;

#ifndef USE_OMS_SSD1306_DRIVER
  gOled2 = new DISPLAY_TYPE((*i2c), PB_5, (0x3C << 1), SCREEN_HEIGHT, SCREEN_WIDTH);
  gOled2->begin(SSD1306_SWITCHCAPVCC);
#else
  gOled2 = new DISPLAY_TYPE(i2c, 0x3C);
#endif

  displaySplash();
}

void printTitle(const char *s, DISPLAY_TYPE *display, const bool newline, const bool print) {
  display->setTextCursor(0, 0);
  display->clearDisplay();
  display->setTextColor(BLACK, WHITE);
  uint8_t len = __strlen(s);
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

void page0(void) {
  printTitle("MAIN", gOled2, true, false);
  //gOled2->printf("T  : %.1f %cC\n\n", temperature, (char)247);
  gOled2->printf("T  : %.1f C\n\nHR : %.1f %%\n\nP  : %.1f hPa", temperature, humidity, pressure);
  gOled2->display();
}

void page1(void) {
  printTitle("Max / Min", gOled2, true, false);

  gOled2->printf("T  max:  %.1f\nT  min:  %.1f\nHR max:  %.1f\nHR min:  %.1f\nP  max:  %.1f\nP  min:  %.1f\n", t_max, t_min, h_max, h_min, p_max, p_min);

  gOled2->display();
}

void page2(void) {
  displayGraph(0, temperatures);
}

void page3(void) {
  displayGraph(1, humidities);
}

void page4(void) {
  displayGraph(2, pressures);
}

void page5(void) {
  printTitle("SAMPLES", gOled2, true, false);
  gOled2->printf("Samples # %d", samples);
  gOled2->display();
}

void refreshDisplay(void) {
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

void displaySplash(void) {
  gOled2->setTextCursor(0, 0);
  gOled2->clearDisplay();
  gOled2->setTextColor(BLACK, WHITE);
  gOled2->printf("     TERMOSTATO      \n");
  gOled2->setTextColor(WHITE, BLACK);
  gOled2->printf("V %d.%d.%d\n\rLoading...", MAJOR, MINOR, REVISION);
  gOled2->display();
//  printTitle("TERMOSTATO", gOled2, true, true);
//  gOled2->printf("V %d.%d.%d\n\r", MAJOR, MINOR, REVISION);
//  gOled2->printf("Niccolo' Ferrari\n\r");
//  gOled2->printf("OMEGASOFTWARE\n\r");
//  gOled2->display();
//  wait_ms(1000);
}