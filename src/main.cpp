/* Example of Reading all the ADC pins PIN and display ot OLED display using
 Nucleo_F303K8. Displays voltage read from first 2 ADC lines on OLED display

  By Joseph Ellsworth CTO of A2WH
  Take a look at A2WH.com Producing Water from Air using Solar Energy
  March-2016 License: https://developer.mbed.org/handbook/MIT-Licence 
  Please contact us http://a2wh.com for help with custom design projects.

  
  Used to Drive:
   ebay part http://www.ebay.com/itm/152000005331?_trksid=p2057872.m2749.l2649&ssPageName=STRK%3AMEBIDX%3AIT
  0-96-I2C-IIC-SPI-Serial-White-OLED-LCD-LED-Display-Module-128X64
  The Datasheet.  https://www.adafruit.com/datasheets/SSD1306.pdf

  Unfortunately this part never turns on using the SSD1308 library.
  but did find that the https://developer.mbed.org/users/nkhorman/code/Adafruit_GFX/
  library works.  Unfortunately the Adafruit library doesn't include the scroll functionality.

  Using my I2C address scanner I found that it responds on Added 120 (x78) ,121 (x79)
  and when the part was unsoldered nothing responded on 120 or 121. According to
  to the page #19 (8.1.5) of the data sheet the I2C address should be 011110
  which seems to map correctly to dec=120 hex=79

 */

#include <mbed.h>
#include <string.h>
#include <Adafruit_SSD1306.h>
#include <BME280.h>

#define D_SDA PB_11
#define D_SCL PB_10
I2C i2c(D_SDA, D_SCL);

DigitalOut statusLed(PC_13);

Adafruit_SSD1306_I2c *gOled2 = NULL;

void printTitle(const char*, Adafruit_SSD1306_I2c*, const bool);

int k = 0;

int main()
{
  wait(1);
  float temp, hum, pres;
  BME280 *bme280 = 0;
  bme280 = new BME280(PB_9, PB_8, 0x76);

  gOled2 = new Adafruit_SSD1306_I2c(i2c, PB_5, (0x3C << 1), 64, 128);
  for (uint8_t i = 0; i < 10; i++)
  {
    gOled2->begin(SSD1306_SWITCHCAPVCC);
    wait(0.01);
  }

  printTitle("TERMOSTATO", gOled2, false);
  
  gOled2->printf("v0.1");
  gOled2->display();
  wait(1);

  bme280->reset();
  wait(2);
  bme280->init();
  wait(1);

  while (1)
  {
    statusLed = !statusLed;
    bme280->trigger();
    while (bme280->busy())
    {
      wait(0.1);
    }
    temp = bme280->getTemperature();
    hum = bme280->getHumidity();
    pres = bme280->getPressure();
    gOled2->clearDisplay();
    gOled2->setTextCursor(0, 0);
    gOled2->printf("Temp: %.2f\n\rHumid: %.2f\n\rPress: %.2f\n\r", temp, hum, pres);
    gOled2->display();
    wait(1);
  }
}

void printTitle(const char *s, Adafruit_SSD1306_I2c *display, const bool print) {
  display->setTextCursor(0, 0);
  display->clearDisplay();
  display->setTextColor(BLACK, WHITE);
  uint8_t len = strlen(s);
  uint8_t x = 21 - len;
  uint8_t h = (uint8_t) (x / 2);
  for (uint8_t i = 0; i < h; i++) {
    display->printf(" ");
  }
  display->printf("%s", s);
  for (uint8_t i = 0; i < h; i++) {
    display->printf(" ");
  }
  if((h + h) < x) {
    display->printf(" ");
  }
  display->printf("\n\r");
  display->setTextColor(WHITE, BLACK);
  if(print) {
    display->display();
  }
}