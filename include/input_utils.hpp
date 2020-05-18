#ifndef _INPUT_UTILS_H_
#define _INPUT_UTILS_H_

#include <mbed.h>
#include "display_utils.hpp"

extern DigitalOut statusLed;

#ifndef USE_INPUT_INTERRUPTS
extern DigitalIn btn_plus;
extern DigitalIn btn_minus;
extern DigitalIn btn_menu;
extern DigitalIn btn_ok;
extern DigitalIn btn_up;
extern DigitalIn btn_down;
#else
extern InterruptIn btn_plus;
extern InterruptIn btn_minus;
extern InterruptIn btn_menu;
extern InterruptIn btn_ok;
extern InterruptIn btn_up;
extern InterruptIn btn_down;
#endif

enum button {
  BTN_PLUS, BTN_MINS, BTN_MENU, BTN_OK, BTN_UP, BTN_DN
};
extern uint8_t btn_click;
extern uint8_t btn_clicked;

extern volatile bool btn_pressed[];

extern uint8_t page;
extern DISPLAY_TYPE *gOled2;

bool begin_input(void);
void readInput(void);
void decodeInput(void);
void waitInput(const uint8_t cycles, const uint16_t ms, bool refresh);

#endif // _INPUT_UTILS_H_