#include "DFRobot_ST7687S.h"
#include "image.h"

DFRobot_ST7687S lcd;

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
  static uint16_t colors[] = {0b1111100000000000, 0b1111111111100000,
                              0b0000011111100000, 0b0000011111111111,
                              0b0000000000011111, 0b1111100000011111};
  static int i = 0;
  i = (i + 1) % (sizeof(colors) / sizeof(*colors));
  lcd.fillScreen(colors[i]);
  lcd.beforeDraw(0, 0, LCD_WIDTH, LCD_HEIGHT);
  for (int y = 0; y < LCD_HEIGHT; ++y)
    for (int x = 0; x < LCD_WIDTH; ++x) lcd.draw(getPixel(0, x, y));
  lcd.afterDraw();
}
