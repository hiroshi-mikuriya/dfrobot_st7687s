#include "DFRobot_ST7687S.h"
#include "image.h"

DFRobot_ST7687S lcd(3, 5, 6, 7);  // CS, RS, WR, LCK

/**
 * @param[in] n image id
 * @param[in] x 0 <= x < LCD_WIDTH
 * @param[in] y 0 <= y < LCD_HEIGHT
 */
uint16_t getPixel(int n, int x, int y) {
  int x0 = x * IMG_WIDTH / LCD_WIDTH;
  int y0 = y * IMG_HEIGHT / LCD_HEIGHT;
  const image_t* img = images[n];
  uint8_t p = pgm_read_byte(img + y0 * IMG_WIDTH + x0);
  uint16_t color = 0;
  color |= (p & 0xC0) << 8;
  color |= (p & 0x38) << 5;
  color |= (p & 0x07) << 2;
  return color;
}

void setup(void) {
  Serial.begin(115200);
  lcd.begin();
}

void loop(void) {
  lcd.fillScreen(0);
  lcd.beforeDraw(0, 0, LCD_WIDTH, LCD_HEIGHT);
  for (int y = 0; y < LCD_HEIGHT; ++y)
    for (int x = 0; x < LCD_WIDTH; ++x) lcd.draw(getPixel(0, x, y));
  lcd.afterDraw();
}
