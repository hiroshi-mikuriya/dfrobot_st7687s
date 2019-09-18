#include "DFRobot_ST7687S.h"
#include "image.h"

DFRobot_ST7687S Lcd;

void setup(void) {
  Serial.begin(115200);
  Lcd.begin();
}

#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F

void loop(void) {
  static int n = 0;
  n = (n + 1) % COUNT_OF_IMAGES;
  const image_t* img = images[n];
  Lcd.beforeDraw(0, 0, LCD_WIDTH, LCD_HEIGHT);
  for (int y = 0; y < LCD_HEIGHT; ++y) {
    for (int x0 = 0; x0 < WIDTH_COUNT; ++x0) {
      for (int x1 = 0; x1 < 5; ++x1) {
        int x = x0 * 5 + x1;
        if (LCD_WIDTH <= x) break;
        uint16_t pix = pgm_read_word(img + y * WIDTH_COUNT + x0);
        uint16_t color = 0;
        if (pix & (1 << (3 * x1 + 2))) color |= RED;
        if (pix & (1 << (3 * x1 + 1))) color |= GREEN;
        if (pix & (1 << (3 * x1 + 0))) color |= BLUE;
        Lcd.draw(color);
      }
    }
  }
  Lcd.afterDraw();
  delay(2000);
}
