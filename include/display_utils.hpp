#ifndef _DISPLAY_UTILS_H_
#define _DISPLAY_UTILS_H_

#include <mbed.h>

#include "SSD1306_OMS_lite.hpp"
#include "constants.h"
#include "peripherals.hpp"
#include "sensors_utils.hpp"
#include "utils.hpp"

class DisplayUtils {
 public:
  DisplayUtils(PeripheralsUtils* peripherals, SensorsUtils* sensor);
  virtual ~DisplayUtils();
  bool start(void);
  void reset(void) { reset(false, false); }
  void reset(bool invert, bool flush);
  void print(const char* string);
  void print(const int32_t number);
  void print(const int number) { this->print((int32_t)number); }
  void print(const float real);
  void flush(void) { gOled2->display(); }

  void printTitle(const char* s);
  void displayGraph(const volatile float* sarray);
  void cycle(void) { this->refreshDisplay(); }

  void incPage(void);
  void degPage(void);
  void incMenu(void);
  void decMenu(void);
  void gotoMenuHome(void);
  void gotoHome(void);

  /**
   *
   * GETTER SECTION
   *  
   **/
  uint8_t getPage(void) const { return this->page; }
  uint8_t getMenu(void) const { return this->menu; }

 protected:
 private:
  void begin(void);
  void displaySplash(void);
  void page0(void);
  void page1(void);
  void page2(void);
  void page3(void);
  void page4(void);
  void page5(void);
  void main_menu(void);
  void refreshDisplay(void);

  SSD1306_OMS_lite* gOled2;
  // Page
  uint8_t page;
  // Menu
  uint8_t menu;
  static const uint8_t fire[];
  PeripheralsUtils* peripherals;
  SensorsUtils* sensor;
};

#endif  // _DISPLAY_UTILS_H_