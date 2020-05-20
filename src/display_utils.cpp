#include "display_utils.hpp"

#include "wifi_utils.hpp"

DISPLAY_TYPE *gOled2 = NULL;

const uint8_t fire[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE1, 0xFE, 0xFC, 0xF8, 0xF8,
    0xF0, 0xE0, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xE0,
    0xF0, 0xFC, 0xFE, 0x7F, 0xFF, 0xFF, 0xFF, 0xF3, 0x07, 0x0F, 0x1F, 0x3F,
    0x7F, 0xFF, 0xFE, 0xFC, 0xF0, 0x80, 0x00, 0x7F, 0xFF, 0xFF, 0x3F, 0x0F,
    0x01, 0x00, 0x03, 0x0F, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07,
    0xFF, 0xFF, 0x3F, 0x00, 0x00, 0x01, 0x07, 0x08, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Page
uint8_t page = 0;

// Menu
uint8_t menu = 0;

bool begin_display(void) {
  gOled2 = new DISPLAY_TYPE(i2c, 0x3C);

  displaySplash();

  return (gOled2 != NULL);
}

void printTitle(const char *s) {
  gOled2->setTextCursor(0, 0);
  gOled2->clearDisplay();
  gOled2->setTextColor(BLACK, WHITE);
  gOled2->printString(s);
  gOled2->setTextColor(WHITE);
}

void displayGraph(const volatile float *sarray) {
  uint8_t i_stamp = samples;

  gOled2->drawFastVLine(GRAPH_X, GRAPH_Y, GRAPH_H, WHITE);
  gOled2->drawFastHLine(GRAPH_X, GRAPH_H + GRAPH_Y, GRAPH_W, WHITE);

  // int mean = 0;
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
  // mean = (int) (mean / i_stamp);
  mean = ((_max + _min) / 2);
  float max_delta = __max((abs(mean - _max)), (abs(mean - _min)));

  uint8_t x = 0;
  if (max_delta != 0)
    x = (uint8_t)((GRAPH_Y + (GRAPH_H / 2)) +
                  (((GRAPH_H / 2) * (-_max + mean)) / max_delta));
  else
    x = GRAPH_Y + (GRAPH_H / 2);

  gOled2->drawPixel(GRAPH_X - 1, x, WHITE);
  gOled2->setTextCursor(0, x);
  gOled2->printFloat(_max);

  if (max_delta != 0)
    x = (uint8_t)((GRAPH_Y + (GRAPH_H / 2)) +
                  (((GRAPH_H / 2) * (-_min + mean)) / max_delta));
  else
    x = GRAPH_Y + (GRAPH_H / 2);
  gOled2->drawPixel(GRAPH_X - 1, x, WHITE);
  gOled2->setTextCursor(0, x - 6);
  gOled2->printFloat(_min);

  gOled2->drawPixel(GRAPH_X - 1, GRAPH_Y + (GRAPH_H / 2), WHITE);
  gOled2->setTextCursor(0, GRAPH_Y + (GRAPH_H / 2) - 3);
  gOled2->printFloat(mean);

  for (uint8_t i = 0; i < i_stamp; i++) {
    float _t = __round(sarray[i]);
    x = (uint8_t)((GRAPH_Y + (GRAPH_H / 2)) +
                  (((GRAPH_H / 2) * (-_t + mean)) / max_delta));
    gOled2->drawPixel(GRAPH_X + i + 1, x, WHITE);
  }

  gOled2->display();
}

void page0(void) {
  printTitle("        MAIN         \n");

  gOled2->printString("Temperatura:\n");
  gOled2->printFloat(temperature);
  gOled2->printString(" C\n\n");
  /*gOled2->printString("Impostata:\n");
  gOled2->printFloat(temperature);
  gOled2->printString(" C");*/
  gOled2->printString("QueryState: ");
  gOled2->printInt(queryState);
  gOled2->printString("\r\n");
  gOled2->printString(schedule);
  gOled2->drawBitmap(100, 16, fire, 21, 30, WHITE);
  gOled2->display();
}

void page1(void) {
  printTitle("        MAIN         \n");
  gOled2->printString("T  : ");
  gOled2->printFloat(temperature);
  gOled2->printString(" C\n\nHR : ");
  gOled2->printFloat(humidity);
  gOled2->printString(" %\n\nP  : ");
  gOled2->printFloat(pressure);
  gOled2->printString(" hPa");
  gOled2->display();
}

void page2(void) {
  printTitle("      Max / Min      \n");
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
  gOled2->display();
}

void page3(void) {
  printTitle("     TEMPERATURA     ");
  displayGraph(temperatures);
}

void page4(void) {
  printTitle("      UMIDITA'       ");
  displayGraph(humidities);
}

void page5(void) {
  printTitle("      PRESSIONE      ");
  displayGraph(pressures);
}

void main_menu() {
  printTitle("        MENU         ");
  gOled2->writeChar((page == 0) ? '>' : ' ');
  gOled2->printString("Temperatura\n");
  gOled2->writeChar((page == 1) ? '>' : ' ');
  gOled2->printString("WIFI\n");
  gOled2->writeChar((page == 2) ? '>' : ' ');
  gOled2->printString("Data e ora\n");
  gOled2->writeChar((page == 3) ? '>' : ' ');
  gOled2->printString("Esci");
  gOled2->display();
}

void refreshDisplay(void) {
  if (menu == 0) {
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
  } else {
    switch (menu) {
      case 1:
        main_menu();
        break;
      case 2:
        break;
      case 3:
        break;
      case 4:
        break;
      case 5:
        break;
      default:
        page = 0;
        menu = 0;
        refreshDisplay();
        break;
    }
  }
}

void displaySplash(void) {
  gOled2->setTextCursor(0, 0);
  gOled2->clearDisplay();
  gOled2->setTextColor(BLACK, WHITE);
  gOled2->printString("  TERMOSTATO V");
  gOled2->printInt(MAJOR);
  gOled2->printString(".");
  gOled2->printInt(MINOR);
  gOled2->printString(".");
  gOled2->printInt(REVISION);
  gOled2->printString("  ");
  gOled2->setTextColor(WHITE, BLACK);
  gOled2->display();
}