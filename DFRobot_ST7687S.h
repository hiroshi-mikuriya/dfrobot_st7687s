#include <stdint.h>

class DFRobot_ST7687S {
 public:
  DFRobot_ST7687S(int16_t width, int16_t height, uint8_t pin_cs, uint8_t pin_cd,
                  uint8_t pin_wr, uint8_t pin_rck);
  int16_t begin();
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void setCursorAddr(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
  void fillScreen(uint16_t color);

 private:
  const uint8_t m_cs;
  const uint8_t m_cd;
  const uint8_t m_wr;
  const uint8_t m_rck;
  const int16_t m_width;
  const int16_t m_height;
  int16_t m_cursorX;
  int16_t m_cursorY;

  int16_t limitPixel(int16_t& x, int16_t& y);
  void setCursor(int16_t x, int16_t y);
  void writeCmd(uint8_t cmd);
  void writeDat(uint8_t dat);
  void writeDatBytes(uint8_t* pDat, uint16_t count);
  void writeRepeatPixel(uint16_t color, uint16_t count, uint16_t repeatCount);
  void writeToRam(void) { writeCmd(0x2c); }
};
