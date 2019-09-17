#include <stdint.h>

#define LCD_WIDTH 128
#define LCD_HEIGHT 128

struct DFRobot_ST7687S {
  void begin() const;

  void fillScreen(uint16_t color) const;

  void beforeDraw(uint16_t x, uint16_t y, uint16_t w, uint16_t h) const;

  void draw(uint8_t* p, uint16_t count) const;

  void draw(uint16_t color) const;

  void afterDraw() const;
};
