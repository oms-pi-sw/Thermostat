#include "input_utils.hpp"

InputUtils::InputUtils(DisplayUtils* display) :
#ifndef USE_INPUT_INTERRUPTS
                                                btn_plus(PA_0),
                                                btn_minus(PA_1),
                                                btn_menu(PA_6),
                                                btn_ok(PA_4),
                                                btn_up(PA_5),
                                                btn_down(PA_7)
#else
                                                btn_plus(PA_0),
                                                btn_minus(PA_1),
                                                btn_menu(PA_6),
                                                btn_ok(PA_4),
                                                btn_up(PA_5),
                                                btn_down(PA_7)
#endif
{
  this->display = display;

  btn_click = 0x00;
  btn_clicked = 0x00;

  begin();
}

InputUtils::~InputUtils() {}

void InputUtils::begin(void) {}

bool InputUtils::start(void) {
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
  return (true);
}

void InputUtils::cycle(void) {
  waitInput(CYCLES, WAIT_MS, false);

  if (btn_pressed[BTN_DN]) {
    btn_pressed[BTN_DN] = false;
    display->incPage();
  }
  if (btn_pressed[BTN_UP]) {
    btn_pressed[BTN_UP] = false;
    display->degPage();
  }
  if (btn_pressed[BTN_MENU]) {
    btn_pressed[BTN_MENU] = false;
    display->gotoMenuHome();
  }
  if (btn_pressed[BTN_OK]) {
    btn_pressed[BTN_OK] = false;
    if (display->getMenu() == MENU_MAIN && display->getPage() == MENU_S_EXIT) {
      display->gotoHome();
    }
  }
}

void InputUtils::pressPlus(void) { btn_pressed[BTN_PLUS] = true; }

void InputUtils::pressMinus(void) { btn_pressed[BTN_MINS] = true; }

void InputUtils::pressMenu(void) { btn_pressed[BTN_MENU] = true; }

void InputUtils::pressOk(void) { btn_pressed[BTN_OK] = true; }

void InputUtils::pressUp(void) { btn_pressed[BTN_UP] = true; }

void InputUtils::pressDown(void) { btn_pressed[BTN_DN] = true; }

void InputUtils::readInput(void) {
  bool plus = btn_plus;
  bool minus = btn_minus;
  bool menu = btn_menu;
  bool ok = btn_ok;
  bool up = btn_up;
  bool down = btn_down;

  btn_clicked = btn_click;

  btn_click = (uint8_t)0x00;

  btn_click |= ((plus & ((uint8_t)0x01)) << BTN_PLUS);
  btn_click |= ((minus & ((uint8_t)0x01)) << BTN_MINS);
  btn_click |= ((menu & ((uint8_t)0x01)) << BTN_MENU);
  btn_click |= ((ok & ((uint8_t)0x01)) << BTN_OK);
  btn_click |= ((up & ((uint8_t)0x01)) << BTN_UP);
  btn_click |= ((down & ((uint8_t)0x01)) << BTN_DN);
}

void InputUtils::decodeInput(void) {
  readInput();

  if ((((btn_click >> BTN_DN) & ((uint8_t)0x01)) == 1) &&
      (((btn_clicked >> BTN_DN) & ((uint8_t)0x01)) == 0)) {
    btn_pressed[BTN_DN] = true;
  }
  if ((((btn_click >> BTN_UP) & ((uint8_t)0x01)) == 1) &&
      (((btn_clicked >> BTN_UP) & ((uint8_t)0x01)) == 0)) {
    btn_pressed[BTN_UP] = true;
  }

  if ((((btn_click >> BTN_MENU) & ((uint8_t)0x01)) == 1) &&
      (((btn_clicked >> BTN_MENU) & ((uint8_t)0x01)) == 0)) {
    btn_pressed[BTN_MENU] = true;
  }
  if ((((btn_click >> BTN_OK) & ((uint8_t)0x01)) == 1) &&
      (((btn_clicked >> BTN_OK) & ((uint8_t)0x01)) == 0)) {
    btn_pressed[BTN_OK] = true;
  }

  if ((((btn_click >> BTN_PLUS) & ((uint8_t)0x01)) == 1) &&
      (((btn_clicked >> BTN_PLUS) & ((uint8_t)0x01)) == 0)) {
    btn_pressed[BTN_PLUS] = true;
  }
  if ((((btn_click >> BTN_MINS) & ((uint8_t)0x01)) == 1) &&
      (((btn_clicked >> BTN_MINS) & ((uint8_t)0x01)) == 0)) {
    btn_pressed[BTN_MINS] = true;
  }
}

void InputUtils::waitInput(const uint8_t cycles, const uint16_t ms, bool refresh) {
  for (uint8_t i = 0; i < cycles; i++) {
#ifndef USE_INPUT_INTERRUPTS
    decodeInput();
#else
    // Nothng todo
#endif
    if (refresh) display->cycle();
    wait_ms(ms);
  }
}

