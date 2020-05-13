#include "input_utils.hpp"

DigitalOut statusLed(PC_13);

#ifndef USE_INPUT_INTERRUPTS
DigitalIn btn_plus(PA_0);
DigitalIn btn_minus(PA_1);
DigitalIn btn_menu(PA_6);
DigitalIn btn_ok(PA_4);
DigitalIn btn_up(PA_5);
DigitalIn btn_down(PA_7);
#else
InterruptIn btn_plus(PA_0);
InterruptIn btn_minus(PA_1);
InterruptIn btn_menu(PA_6);
InterruptIn btn_ok(PA_4);
InterruptIn btn_up(PA_5);
InterruptIn btn_down(PA_7);
#endif

uint8_t btn_click = 0x00;
uint8_t btn_clicked = 0x00;

volatile bool btn_pressed[6] = {false, false, false, false, false};

void pressPlus(void) {
  btn_pressed[BTN_PLUS] = true;
}

void pressMinus(void) {
  btn_pressed[BTN_MINS] = true;
}

void pressMenu(void) {
  btn_pressed[BTN_MENU] = true;
}

void pressOk(void) {
  btn_pressed[BTN_OK] = true;
}

void pressUp(void) {
  btn_pressed[BTN_UP] = true;
}

void pressDown(void) {
  btn_pressed[BTN_DN] = true;
}

void begin_input(void) {
#ifndef USE_INPUT_INTERRUPTS
  // Fake init
#else
  btn_plus.fall(&pressPlus);
  btn_minus.fall(&pressMinus);
  btn_menu.fall(&pressMenu);
  btn_ok.fall(&pressOk);
  btn_up.fall(&pressUp);
  btn_down.fall(&pressDown);
#endif
}

void readInput(void) {
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

void decodeInput(void) {
  readInput();

  if ((((btn_click >> BTN_DN) & ((uint8_t) 0x01)) == 1) && (((btn_clicked >> BTN_DN) & ((uint8_t) 0x01)) == 0)) {
    btn_pressed[BTN_DN] = true;
  }
  if ((((btn_click >> BTN_UP) & ((uint8_t) 0x01)) == 1) && (((btn_clicked >> BTN_UP) & ((uint8_t) 0x01)) == 0)) {
    btn_pressed[BTN_UP] = true;
  }
}

void waitInput(const uint8_t cycles, const uint16_t ms, bool refresh){
  for (uint8_t i = 0; i < cycles; i++) {
#ifndef USE_INPUT_INTERRUPTS
    decodeInput();
#else
  // Nothng todo
#endif
    if(refresh) refreshDisplay();
    wait_ms(ms);
  }
}


