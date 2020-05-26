#include "display_utils.hpp"

const uint8_t DisplayUtils::fire[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE1, 0xFE, 0xFC, 0xF8, 0xF8,
    0xF0, 0xE0, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xE0,
    0xF0, 0xFC, 0xFE, 0x7F, 0xFF, 0xFF, 0xFF, 0xF3, 0x07, 0x0F, 0x1F, 0x3F,
    0x7F, 0xFF, 0xFE, 0xFC, 0xF0, 0x80, 0x00, 0x7F, 0xFF, 0xFF, 0x3F, 0x0F,
    0x01, 0x00, 0x03, 0x0F, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07,
    0xFF, 0xFF, 0x3F, 0x00, 0x00, 0x01, 0x07, 0x08, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

DisplayUtils::DisplayUtils(PeripheralsUtils *peripherals, SensorsUtils *sensor) {
  this->peripherals = peripherals;
  this->sensor = sensor;

  begin();
}

DisplayUtils::~DisplayUtils() {}

void DisplayUtils::begin(void) {
  gOled2 = new SSD1306_OMS_lite(peripherals->getI2C(), 0x3C);
}

bool DisplayUtils::start(void) {
  if (gOled2 == nullptr) return (false);

  displaySplash();

  gotoHome();

  return (true);
}

void DisplayUtils::reset(bool invert, bool flush) {
  gOled2->reset(flush);
  if (invert)
    gOled2->setTextColor(BLACK, WHITE);
  else
    gOled2->setTextColor(WHITE, BLACK);
}

void DisplayUtils::print(const char *string) { this->gOled2->printString(string); }
void DisplayUtils::print(const int32_t number) { gOled2->printInt(number); }
void DisplayUtils::print(const float real) { gOled2->printFloat(real); }

void DisplayUtils::printTitle(const char *s) {
  reset(true, false);
  print(s);
  gOled2->setTextColor(WHITE, BLACK);
}

void DisplayUtils::displayGraph(const volatile float *sarray) {
  uint8_t i_stamp = this->sensor->getSamples();

  gOled2->drawFastVLine(GRAPH_X, GRAPH_Y, GRAPH_H, WHITE);
  gOled2->drawFastHLine(GRAPH_X, GRAPH_H + GRAPH_Y, GRAPH_W, WHITE);

  // int mean = 0;
  float mean = 0;
  float _max = Utils::__round(sarray[0]);
  float _min = Utils::__round(sarray[0]);

  for (uint8_t i = 0; i < i_stamp; i++) {
    if (_max < sarray[i]) {
      _max = Utils::__round(sarray[i]);
    }
    if (_min > sarray[i]) {
      _min = Utils::__round(sarray[i]);
    }
  }
  // mean = (int) (mean / i_stamp);
  mean = ((_max + _min) / 2);
  float max_delta = Utils::__max((abs(mean - _max)), (abs(mean - _min)));

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
    float _t = Utils::__round(sarray[i]);
    x = (uint8_t)((GRAPH_Y + (GRAPH_H / 2)) +
                  (((GRAPH_H / 2) * (-_t + mean)) / max_delta));
    gOled2->drawPixel(GRAPH_X + i + 1, x, WHITE);
  }

  gOled2->display();
}

void DisplayUtils::page0(void) {
  printTitle("        MAIN         \n");

  print("Temperatura:\n");
  print(this->sensor->getTemperature());
  print(" C\n\n");
  print("QueryState: ");
  //print(queryState);
  print("SampleState: ");
  //print(sampleState);
  print("\r\n");
  //print(schedule);
  gOled2->drawBitmap(100, 16, fire, 21, 30, WHITE);
  flush();
}

void DisplayUtils::page1(void) {
  printTitle("        MAIN         \n");
  gOled2->printString("T  : ");
  gOled2->printFloat(this->sensor->getTemperature());
  gOled2->printString(" C\n\nHR : ");
  gOled2->printFloat(this->sensor->getHumidity());
  gOled2->printString(" %\n\nP  : ");
  gOled2->printFloat(this->sensor->getPressure());
  gOled2->printString(" hPa");
  gOled2->display();
}

void DisplayUtils::page2(void) {
  printTitle("      Max / Min      \n");
  gOled2->printString("T  max:  ");
  gOled2->printFloat(this->sensor->getTMax());
  gOled2->printString("\nT  min:  ");
  gOled2->printFloat(this->sensor->getTMin());
  gOled2->printString("\nH  max:  ");
  gOled2->printFloat(this->sensor->getHMax());
  gOled2->printString("\nH  min:  ");
  gOled2->printFloat(this->sensor->getHMin());
  gOled2->printString("\nP  max:  ");
  gOled2->printFloat(this->sensor->getPMax());
  gOled2->printString("\nP  min:  ");
  gOled2->printFloat(this->sensor->getPMin());
  gOled2->display();
}

void DisplayUtils::page3(void) {
  printTitle("     TEMPERATURA     ");
  displayGraph(this->sensor->getTemperatures());
}

void DisplayUtils::page4(void) {
  printTitle("      UMIDITA'       ");
  displayGraph(this->sensor->getHumidities());
}

void DisplayUtils::page5(void) {
  printTitle("      PRESSIONE      ");
  displayGraph(this->sensor->getPressures());
}

void DisplayUtils::main_menu() {
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

void DisplayUtils::refreshDisplay(void) {
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

void DisplayUtils::displaySplash(void) {
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

void DisplayUtils::incPage(void) {
  if ((!menu && page < 5) || (menu && page < 3)) ++page;
}
void DisplayUtils::degPage(void) {
  if (page > 0) --page;
}
void DisplayUtils::incMenu(void) {}
void DisplayUtils::decMenu(void) {}
void DisplayUtils::gotoMenuHome(void) {
  this->menu = MENU_MAIN;
  this->page = MENU_S_TEMPERATURE;
}
void DisplayUtils::gotoHome(void) {
  this->menu = MENU_NO;
  this->page = PAGE_MAIN;
}