#ifndef _INPUT_UTILS_H_
#define _INPUT_UTILS_H_

#include <mbed.h>

#include "display_utils.hpp"

class InputUtils {
 public:
  InputUtils(DisplayUtils* display);
  virtual ~InputUtils();
  bool start(void);
  void cycle(void);

 protected:
 private:
  void begin(void);
  void pressPlus(void);
  void pressMinus(void);
  void pressMenu(void);
  void pressOk(void);
  void pressUp(void);
  void pressDown(void);

  void readInput(void);
  void decodeInput(void);
  void waitInput(const uint8_t cycles, const uint16_t ms, bool refresh);

  enum button {
    BTN_PLUS,
    BTN_MINS,
    BTN_MENU,
    BTN_OK,
    BTN_UP,
    BTN_DN
  };

  volatile bool btn_pressed[6] = {false, false, false, false, false};
  uint8_t btn_click = 0x00;
  uint8_t btn_clicked = 0x00;

  DisplayUtils* display;

#ifndef USE_INPUT_INTERRUPTS
  DigitalIn btn_plus;
  DigitalIn btn_minus;
  DigitalIn btn_menu;
  DigitalIn btn_ok;
  DigitalIn btn_up;
  DigitalIn btn_down;
#else
  InterruptIn btn_plus;
  InterruptIn btn_minus;
  InterruptIn btn_menu;
  InterruptIn btn_ok;
  InterruptIn btn_up;
  InterruptIn btn_down;
#endif
};

#endif  // _INPUT_UTILS_H_