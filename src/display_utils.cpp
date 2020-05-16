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

#ifndef USE_OMS_SSD1306_DRIVER_LITE
void printTitle(const char *s, DISPLAY_TYPE *display, const bool newline, const bool print) {
  display->setTextCursor(0, 0);
  display->clearDisplay();
  display->setTextColor(BLACK, WHITE);
  uint8_t len = __strlen(s);
  uint8_t x = 21 - len;
  uint8_t h = (uint8_t) (x / 2);
  for (uint8_t i = 0; i < h; i++) {
#ifndef USE_OMS_SSD1306_DRIVER_LITE
    display->printf(" ");
#else
  display->printString(" ");
#endif
  }

#ifndef USE_OMS_SSD1306_DRIVER_LITE
  display->printf("%s", s);
#else
  display->printString(s);
#endif

  for (uint8_t i = 0; i < h; i++) {
#ifndef USE_OMS_SSD1306_DRIVER_LITE
    display->printf(" ");
#else
  display->printString(" ");
#endif
  }
  if((h + h) < x) {
#ifndef USE_OMS_SSD1306_DRIVER_LITE
    display->printf(" ");
#else
  display->printString(" ");
#endif
  }
  if(newline) {
#ifndef USE_OMS_SSD1306_DRIVER_LITE
    display->printf("\n\r");
#else
  display->printString("\n\r");
#endif
  }
  display->setTextColor(WHITE, BLACK);
  if(print) {
    display->display();
  }
}
#else
void printTitle(const char *s) {
  gOled2->setTextCursor(0, 0);
  gOled2->clearDisplay();
  gOled2->setTextColor(BLACK, WHITE);
  gOled2->printString(s);
  gOled2->setTextColor(WHITE);
}
#endif

void displayGraph(const volatile float *sarray) {
  uint8_t i_stamp = samples;    

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
#ifndef USE_OMS_SSD1306_DRIVER_LITE
  gOled2->printf("%.1f", _max);
#else
  gOled2->printFloat(_max);
#endif

  if(max_delta != 0)
    x = (uint8_t) ((GRAPH_Y + (GRAPH_H / 2)) + (((GRAPH_H / 2) * (- _min + mean)) / max_delta));
  else
    x = GRAPH_Y + (GRAPH_H / 2);
  gOled2->drawPixel(GRAPH_X - 1, x, WHITE);
  gOled2->setTextCursor(0, x - 6);
#ifndef USE_OMS_SSD1306_DRIVER_LITE
  gOled2->printf("%.1f", _min);
#else
  gOled2->printFloat(_min);
#endif

  gOled2->drawPixel(GRAPH_X - 1, GRAPH_Y + (GRAPH_H / 2), WHITE);
  gOled2->setTextCursor(0,  GRAPH_Y + (GRAPH_H / 2) - 3);
#ifndef USE_OMS_SSD1306_DRIVER_LITE
  gOled2->printf("%.1f", mean);
#else
  gOled2->printFloat(mean);
#endif

  for (uint8_t i = 0; i < i_stamp; i++) {
    float _t = __round(sarray[i]);
    x = (uint8_t) ((GRAPH_Y + (GRAPH_H / 2)) + (((GRAPH_H / 2) * (- _t + mean)) / max_delta));
    gOled2->drawPixel(GRAPH_X + i + 1, x, WHITE);
  }

  gOled2->display();
}

void page0(void) {
#ifndef USE_OMS_SSD1306_DRIVER_LITE
  printTitle("MAIN", gOled2, true, false);
#else
  printTitle("        MAIN         \n");
#endif

#ifndef USE_OMS_SSD1306_DRIVER_LITE
  gOled2->printf("T  : %.1f C\n\nHR : %.1f %%\n\nP  : %.1f hPa", temperature, humidity, pressure);
#else
  gOled2->printString("T  : ");
  gOled2->printFloat(temperature);
  gOled2->printString(" C\n\nHR : ");
  gOled2->printFloat(humidity);
  gOled2->printString(" %\n\nP  : ");
  gOled2->printFloat(pressure);
  gOled2->printString(" hPa");
#endif
  gOled2->display();
}

void page1(void) {
#ifndef USE_OMS_SSD1306_DRIVER_LITE
  printTitle("Max / Min", gOled2, true, false);
#else
  printTitle("      Max / Min      \n");
#endif

#ifndef USE_OMS_SSD1306_DRIVER_LITE
  gOled2->printf("T  max:  %.1f\nT  min:  %.1f\nHR max:  %.1f\nHR min:  %.1f\nP  max:  %.1f\nP  min:  %.1f\n", t_max, t_min, h_max, h_min, p_max, p_min);
#else
  gOled2->printString("T  max:  ");
  gOled2->printFloat(t_max);
  gOled2->printString("\nT  min:  ");
  gOled2->printFloat(t_min);
  gOled2->printString("\nH  max:  ");
  gOled2->printFloat(h_max);
  gOled2->printString("\nH  min:  ");
  gOled2->printFloat(h_min);
  gOled2->printString("\nP  max:  ");
  gOled2->printFloat(p_max);
  gOled2->printString("\nP  min:  ");
  gOled2->printFloat(p_min);
#endif
  gOled2->display();
}

void page2(void) {
#ifndef USE_OMS_SSD1306_DRIVER_LITE
  printTitle("TEMPERATURA (C)", gOled2, false, false);
#else
  printTitle("     TEMPERATURA     ");
#endif

  displayGraph(temperatures);
}

void page3(void) {
#ifndef USE_OMS_SSD1306_DRIVER_LITE
  printTitle("UMIDITA' (%)", gOled2, false, false);
#else
  printTitle("      UMIDITA'       ");
#endif

  displayGraph(humidities);
}

void page4(void) {
#ifndef USE_OMS_SSD1306_DRIVER_LITE
  printTitle("PRESSIONE (hPa)", gOled2, false, false);
#else
  printTitle("      PRESSIONE      ");
#endif

  displayGraph(pressures);
}

void page5(void) {
#ifndef USE_OMS_SSD1306_DRIVER_LITE
  printTitle("SAMPLES", gOled2, true, false);
#else
  printTitle("       SAMPLES       \n");
#endif
  
#ifndef USE_OMS_SSD1306_DRIVER_LITE
  gOled2->printf("Samples # %d", samples);
#else
  gOled2->printString("Samples # ");
  gOled2->printInt(samples);
#endif
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
#ifndef USE_OMS_SSD1306_DRIVER_LITE
  gOled2->printf("     TERMOSTATO      \n");
#else
  gOled2->printString("     TERMOSTATO      \n");
#endif
  gOled2->setTextColor(WHITE, BLACK);
#ifndef USE_OMS_SSD1306_DRIVER_LITE
  gOled2->printf("V %d.%d.%d\n\rLoading...", MAJOR, MINOR, REVISION);
#else
  gOled2->printString("V ");
  gOled2->printInt(MAJOR);
  gOled2->printString(".");
  gOled2->printInt(MINOR);
  gOled2->printString(".");
  gOled2->printInt(REVISION);
#endif
  gOled2->display();
}