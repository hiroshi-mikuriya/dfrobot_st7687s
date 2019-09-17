#include <stdint.h>

#define LCD_WIDTH 128
#define LCD_HEIGHT 128

class DFRobot_ST7687S {
 public:
  DFRobot_ST7687S(uint8_t cs, uint8_t rs, uint8_t wr, uint8_t lck);
  void begin() const;
  void setCursorAddr(int16_t x0, int16_t y0, int16_t x1, int16_t y1) const;
  void fillScreen(uint16_t color) const;

  void beforeDraw(uint16_t x, uint16_t y, uint16_t w, uint16_t h) const;
  void draw(uint8_t* p, uint16_t count) const;
  void draw(uint16_t color) const;
  void afterDraw() const;

 private:
  const uint8_t m_cs;
  const uint8_t m_rs;
  const uint8_t m_wr;
  const uint8_t m_lck;

  void writeCmd(uint8_t cmd) const;
  void writeDat(uint8_t dat) const;
  void writeDatBytes(uint8_t* pDat, uint16_t count) const;
  void writeRepeatPixel(uint16_t color, uint16_t count,
                        uint16_t repeatCount) const;
  void writeToRam(void) const { writeCmd(0x2c); }
};
