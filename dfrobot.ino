#include "DFRobot_ST7687S.h"
#include "image.h"

DFRobot_ST7687S Lcd;

void setup(void) {
  Serial.begin(115200);
  Lcd.begin();
}

void loop(void) {
  Lcd.beforeDraw(0, 0, LCD_WIDTH, LCD_HEIGHT);
  for (uint16_t i = 0; i < sizeof(image2); i += 2) {
    uint8_t p = pgm_read_byte(image2 + i);
    uint8_t count = pgm_read_byte(image2 + i + 1);
    uint16_t color = 0;
    color |= ((p & 0xC0) | 0x38) << 8;
    color |= ((p & 0x38) | 0x07) << 5;
    color |= ((p & 0x07) << 2) | 0x03;
    for (int j = 0; j < count; ++j) Lcd.draw(color);
  }
  Lcd.afterDraw();
  delay(2000);
}
