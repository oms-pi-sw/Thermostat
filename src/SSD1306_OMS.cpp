#include "SSD1306_OMS.hpp"

static void __swap(int16_t &a, int16_t &b) {
  int16_t t = a;

  a = b;
  b = t;
}

void SSD1306_OMS::clearDisplay(void) {
#ifndef USE_MEMSET
  for (uint16_t i = 0; i < BUFFER_LEN; i++) {
    buffer[i] = 0x0;
  }
#else
  memset(buffer, 0, BUFFER_LEN * sizeof(uint8_t));
#endif
}

void SSD1306_OMS::display(void) {
  command(0x22);
  command(0x00);
  command(0xFF);
  command(0x21);
  command(0x00);
  command(SCREEN_WIDTH - 1);

  char buff[17];
  buff[0] = 0x40;  // Data Mode

  // send display buffer in 16 byte chunks
  for (uint16_t i = 0, q = BUFFER_LEN; i < q; i += 16) {
    uint8_t x = 0x0;

    // TODO - this will segfault if buffer.size() % 16 != 0
    for (x = 1; x < sizeof(buff); x++) buff[x] = buffer[i + x - 1];
    mi2c->write(mi2cAddress, buff, sizeof(buff));
  }
}

void SSD1306_OMS::command(uint8_t c) {
  char buff[2];
  buff[0] = 0;  // Command Mode
  buff[1] = c;
  mi2c->write(mi2cAddress, buff, sizeof(buff));
}

void SSD1306_OMS::data(uint8_t c) {
  char buff[2];
  buff[0] = 0x40;  // Data Mode
  buff[1] = c;
  mi2c->write(mi2cAddress, buff, sizeof(buff));
};

size_t SSD1306_OMS::writeChar(uint8_t c) {
  if (c == '\n') {
    cursor_y += TEXT_SIZE * 8;
    cursor_x = 0;
  } else if (c == '\r')
    cursor_x = 0;
  else {
    drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, TEXT_SIZE);
    cursor_x += TEXT_SIZE * 6;
    if (WRAP && (cursor_x > (SCREEN_WIDTH - TEXT_SIZE * 6))) {
      cursor_y += TEXT_SIZE * 8;
      cursor_x = 0;
    }
  }
  return 1;
}

void SSD1306_OMS::drawChar(int16_t x, int16_t y, unsigned char c,
                           uint16_t color, uint16_t bg, uint8_t size) {
  if ((x >= SCREEN_WIDTH) ||       // Clip right
      (y >= SCREEN_HEIGHT) ||      // Clip bottom
      ((x + 5 * size - 1) < 0) ||  // Clip left
      ((y + 8 * size - 1) < 0)     // Clip top
  )
    return;

  for (int8_t i = 0; i < 6; i++) {
    uint8_t line = 0;

    if (i == 5)
      line = 0x0;
    else
      line = font[((c - OFFSET) * 5) + i];

    for (int8_t j = 0; j < 8; j++) {
      if (line & 0x1) {
        if (size == 1)  // default size
          drawPixel(x + i, y + j, color);
        // else  // big size
        //  fillRect(x + (i * size), y + (j * size), size, size, color);
      } else if (bg != color) {
        if (size == 1)  // default size
          drawPixel(x + i, y + j, bg);
        // else  // big size
        //  fillRect(x + i * size, y + j * size, size, size, bg);
      }
      line >>= 1;
    }
  }
}

void SSD1306_OMS::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((x < 0) || (x >= SCREEN_WIDTH) || (y < 0) || (y >= SCREEN_HEIGHT)) return;

  // check rotation, move pixel around if necessary
  // switch (getRotation()) {
  //  case 1:
  //    __swap(x, y);
  //    x = SCREEN_WIDTH - x - 1;
  //    break;
  //  case 2:
  //    x = SCREEN_WIDTH - x - 1;
  //    y = SCREEN_HEIGHT - y - 1;
  //    break;
  //  case 3:
  //    __swap(x, y);
  //    y = SCREEN_HEIGHT - y - 1;
  //    break;
  //}

  // x is which column
  if (color == WHITE)
    buffer[x + (y / 8) * SCREEN_WIDTH] |= _BV((y % 8));
  else  // else black
    buffer[x + (y / 8) * SCREEN_WIDTH] &= ~_BV((y % 8));
}

void SSD1306_OMS::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                           uint16_t color) {
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);

  if (steep) {
    __swap(x0, y0);
    __swap(x1, y1);
  }

  if (x0 > x1) {
    __swap(x0, x1);
    __swap(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1)
    ystep = 1;
  else
    ystep = -1;

  for (; x0 <= x1; x0++) {
    if (steep)
      drawPixel(y0, x0, color);
    else
      drawPixel(x0, y0, color);

    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

void SSD1306_OMS::drawFastVLine(int16_t x, int16_t y, int16_t h,
                                uint16_t color) {
  drawLine(x, y, x, y + h - 1, color);
}

void SSD1306_OMS::drawFastHLine(int16_t x, int16_t y, int16_t w,
                                uint16_t color) {
  drawLine(x, y, x + w - 1, y, color);
}