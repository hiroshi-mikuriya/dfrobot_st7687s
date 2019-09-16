#pragma once

#include <stdint.h>  // uint8_t

class my_st7687s {
  uint8_t rs_;
  uint8_t wr_;
  uint8_t lck_;
  uint8_t cs_;

  void writeCmd(uint8_t cmd) const;

  void writeDat(uint8_t dat) const;

  void writeDatBytes(uint8_t* pDat, uint16_t count) const;

  void writeRepeatPixel(uint16_t color, uint16_t count,
                        uint16_t repeatCount) const;

  void setCursorAddr(int16_t x0, int16_t y0, int16_t x1, int16_t y1) const;

 public:
  my_st7687s(uint8_t rs, uint8_t wr, uint8_t lck, uint8_t cs);

  virtual ~my_st7687s();

  void begin() const;

  void fillScreen(uint16_t color) const;

  void drawPixel(int16_t x, int16_t y, uint16_t color) const;
};