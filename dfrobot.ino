#include "DFRobot_ST7687S.h"
#include "image.h"

DFRobot_ST7687S Lcd;

void setup(void) {
  Serial.begin(115200);
  Lcd.begin();
}

void loop(void) {
  static int n = 0;
  const image_t* img = images[n];
  uint16_t size = image_sizes[n];
  Lcd.beforeDraw(0, 0, LCD_WIDTH, LCD_HEIGHT);
  for (uint16_t i = 0; i < size; i += 2) {
    uint8_t p = pgm_read_byte(img + i);
    uint8_t count = pgm_read_byte(img + i + 1);
    uint16_t color = 0b0011100011100011;
    color |= (p & 0xC0) << 8;
    color |= (p & 0x38) << 5;
    color |= (p & 0x07) << 2;
    for (int j = 0; j < count; ++j) Lcd.draw(color);
  }
  Lcd.afterDraw();
  delay(2000);
  n = (n + 1) % COUNT_OF_IMAGES;
}
