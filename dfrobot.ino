#include "DFRobot_ST7687S.h"
#include "image.h"

const uint8_t pin_cs = 3;
const uint8_t pin_rs = 5;
const uint8_t pin_wr = 6;
const uint8_t pin_lck = 7;

DFRobot_ST7687S lcd(pin_cs, pin_rs, pin_wr, pin_lck);

void setup(void) {
  Serial.begin(115200);
  lcd.begin();
  lcd.fillScreen(0);
  lcd.beforeDraw(0,0,LCD_WIDTH, LCD_HEIGHT);
  for (int y = 0; y < IMG_HEIGHT; ++y) {
    for (int x = 0; x < IMG_WIDTH; ++x) {
      uint8_t p = pgm_read_byte(image1 + y * IMG_WIDTH + x);
      uint16_t color = 0;
      color |= (p & 0xC0) << 8;
      color |= (p & 0x38) << 5;
      color |= (p & 0x07) << 2;
      lcd.draw(color);
    }
  }
  lcd.afterDraw();
}

void loop(void) {}
