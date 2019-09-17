#ifndef INCLUDE_DFROBOT_ST7687S__
#define INCLUDE_DFROBOT_ST7687S__

#include <stdint.h>

#define LCD_WIDTH 128
#define LCD_HEIGHT 128

struct DFRobot_ST7687S {
  void begin() const;

  void fillScreen(uint16_t color) const;

  void beforeDraw(uint8_t x, uint8_t y, uint8_t w, uint8_t h) const;

  void draw(uint8_t const* p, uint16_t count) const;

  void draw(uint16_t color) const;

  void afterDraw() const;
};

#endif  // INCLUDE_DFROBOT_ST7687S__
