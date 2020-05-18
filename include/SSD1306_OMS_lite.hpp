#ifndef _SSD1306_OMS_lite_H_
#define _SSD1306_OMS_lite_H_

#include <mbed.h>

#include "constants.h"
#include "font_oms.h"
#include "utils.hpp"

#define BUFFER_LEN (SCREEN_WIDTH * SCREEN_HEIGHT / 8)

#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif

#define BLACK 0
#define WHITE 1

#define OFFSET 32

#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10
#define SSD1306_SETSTARTLINE 0x40
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SEGREMAP 0xA0
#define SSD1306_CHARGEPUMP 0x8D

#define WRAP true

class SSD1306_OMS_lite {
 public:
  SSD1306_OMS_lite(I2C *i2c, const uint8_t address) {
    this->mi2c = i2c;
    this->mi2cAddress = (address << 1);
    clearDisplay();

    // BEGIN
    // mi2c->start();
    wait_ms(10);

    command(SSD1306_DISPLAYOFF);

    command(SSD1306_SETDISPLAYCLOCKDIV);
    command(0x80);  // the suggested ratio 0x80

    command(SSD1306_SETMULTIPLEX);
    command(SCREEN_HEIGHT - 1);

    command(SSD1306_SETDISPLAYOFFSET);
    command(0x0);  // no offset

    command(SSD1306_SETSTARTLINE | 0x0);  // line #0

    command(SSD1306_CHARGEPUMP);
    command(0x14);

    command(SSD1306_MEMORYMODE);
    command(0x00);  // 0x0 act like ks0108

    command(SSD1306_SEGREMAP | 0x1);

    command(SSD1306_COMSCANDEC);

    command(SSD1306_SETCOMPINS);
    command(0x12);  // TODO - calculate based on _rawHieght ?

    command(SSD1306_SETCONTRAST);
    command(0xCF);

    command(SSD1306_SETPRECHARGE);
    command(0xF1);

    command(SSD1306_SETVCOMDETECT);
    command(0x40);

    command(SSD1306_DISPLAYALLON_RESUME);

    command(SSD1306_NORMALDISPLAY);

    command(0x2E);

    command(0x22);
    command(0x00);
    command(0xFF);
    command(0x21);
    command(0x00);
    command(SCREEN_WIDTH - 1);

    command(SSD1306_DISPLAYON);

    wait_ms(1);
    // END BEGIN

    display();
  }

  void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);

  void display(void);
  void setTextCursor(int16_t x, int16_t y) {
    cursor_x = x;
    cursor_y = y;
  };
  void clearDisplay(void);
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
  void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void drawFastHLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void setTextColor(uint16_t c) {
    textcolor = c;
    textbgcolor = c;
  }
  void setTextColor(uint16_t c, uint16_t b) {
    textcolor = c;
    textbgcolor = b;
  };

  void printString(const char *s);
  void printInt(const int i);
  void printFloat(const float f);

  size_t writeChar(uint8_t c);

 private:
  uint8_t buffer[BUFFER_LEN];
  I2C *mi2c = NULL;
  uint8_t mi2cAddress;
  int16_t cursor_x, cursor_y;
  uint16_t textcolor, textbgcolor;

  void begin(void);
  void command(uint8_t c);
  void data(uint8_t c);
  void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color,
                uint16_t bg);
};

#endif  // _SSD1306_OMS_lite_H_