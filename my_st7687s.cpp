#include "my_st7687s.h"
#include <SPI.h>

void my_st7687s::writeCmd(uint8_t cmd) const {
  digitalWrite(rs_, LOW);
  digitalWrite(cs_, LOW);

  SPI.transfer(cmd);
  digitalWrite(lck_, HIGH);
  digitalWrite(lck_, LOW);

  digitalWrite(wr_, LOW);
  digitalWrite(wr_, HIGH);
  digitalWrite(cs_, HIGH);
}

void my_st7687s::writeDat(uint8_t dat) const {
  digitalWrite(rs_, HIGH);
  digitalWrite(cs_, LOW);

  SPI.transfer(dat);
  digitalWrite(lck_, HIGH);
  digitalWrite(lck_, LOW);

  digitalWrite(wr_, LOW);
  digitalWrite(wr_, HIGH);
  digitalWrite(cs_, HIGH);
}

void my_st7687s::writeDatBytes(uint8_t* pDat, uint16_t count) const {
  digitalWrite(rs_, HIGH);
  digitalWrite(cs_, LOW);
  while (count--) {
    SPI.transfer(*pDat);
    digitalWrite(lck_, HIGH);
    digitalWrite(lck_, LOW);

    digitalWrite(wr_, LOW);
    digitalWrite(wr_, HIGH);
    pDat++;
  }
  digitalWrite(cs_, HIGH);
}

void my_st7687s::writeRepeatPixel(uint16_t color, uint16_t count,
                                  uint16_t repeatCount) const {
  uint8_t buf[2] = {static_cast<uint8_t>(color >> 8),
                    static_cast<uint8_t>(color)};
  for (int32_t i = 0; i < repeatCount * count; i++)
    writeDatBytes(buf, sizeof(buf));
}

void my_st7687s::setCursorAddr(int16_t x0, int16_t y0, int16_t x1,
                               int16_t y1) const {
  uint8_t addrBuf[2] = {(uint8_t)x0, (uint8_t)x1};
  writeCmd(0x2a);
  writeDatBytes(addrBuf, 2);
  addrBuf[0] = (uint8_t)y0;
  addrBuf[1] = (uint8_t)y1;
  writeCmd(0x2b);
  writeDatBytes(addrBuf, 2);
}

my_st7687s::my_st7687s(uint8_t rs, uint8_t wr, uint8_t lck, uint8_t cs)
    : rs_(rs), wr_(wr), lck_(lck), cs_(cs) {
  Serial.println("my_st7687s::my_st7687s");
  pinMode(rs_, OUTPUT);
  digitalWrite(rs_, HIGH);
  pinMode(wr_, OUTPUT);
  digitalWrite(wr_, HIGH);
  pinMode(lck_, OUTPUT);
  digitalWrite(lck_, HIGH);
  pinMode(cs_, OUTPUT);
  digitalWrite(cs_, HIGH);
}

my_st7687s::~my_st7687s() {
  Serial.println("my_st7687s::~my_st7687s");
  SPI.endTransaction();
  SPI.end();
}

void my_st7687s::begin() const {
  Serial.println("my_st7687s::begin");
  SPI.begin();
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));

  delay(120);

  writeCmd(0xd7);
  writeDat(0x9f);

  writeCmd(0xE0);
  writeDat(0x00);
  delay(10);

  writeCmd(0xFA);
  writeDat(0x01);
  delay(20);

  writeCmd(0xE3);
  delay(20);
  writeCmd(0xE1);

  writeCmd(0x28);
  writeCmd(0x11);
  delay(30);
  writeCmd(0xc0);
  writeDat(0x17);  // ctrL=0x1b 080416 5PCS 0X1E; 8PCS 0X2A
  writeDat(0x01);

  writeCmd(0x25);
  writeDat(0x1E);
  writeCmd(0xC3);
  writeDat(0x03);

  writeCmd(0xC4);
  writeDat(0x07);

  writeCmd(0xC5);
  writeDat(0x01);

  writeCmd(0xCB);
  writeDat(0x01);

  writeCmd(0xB7);
  writeDat(0x00);

  writeCmd(0xD0);
  writeDat(0x1d);
  writeCmd(0xB5);
  writeDat(0x89);

  writeCmd(0xBD);
  writeDat(0x02);

  writeCmd(0xF0);
  writeDat(0x07);
  writeDat(0x0C);
  writeDat(0x0C);
  writeDat(0x12);

  writeCmd(0xF4);
  writeDat(0x33);
  writeDat(0x33);
  writeDat(0x33);
  writeDat(0x00);
  writeDat(0x33);
  writeDat(0x66);
  writeDat(0x66);
  writeDat(0x66);

  writeCmd(0x20);
  writeCmd(0x2A);
  writeDat(0x00);
  writeDat(0x7F);

  writeCmd(0x2B);
  writeDat(0x00);
  writeDat(0x7f);

  writeCmd(0x3A);
  writeDat(0x05);

  writeCmd(0x36);
  writeDat(0x80);  // 0xc8

  writeCmd(0xB0);
  writeDat(0x7F);

  writeCmd(0x29);
  ////////
  writeCmd(0xF9);
  writeDat(0x00);
  writeDat(0x02);
  writeDat(0x04);
  writeDat(0x06);
  writeDat(0x08);
  writeDat(0x0a);
  writeDat(0x0c);
  writeDat(0x0e);
  writeDat(0x10);
  writeDat(0x12);
  writeDat(0x14);
  writeDat(0x16);
  writeDat(0x18);
  writeDat(0x1A);
  writeDat(0x1C);
  writeDat(0x1E);

  writeCmd(0x29);
}

void my_st7687s::fillScreen(uint16_t color) const {
  Serial.println("my_st7687s::fillScreen");
  setCursorAddr(0, 0, 128, 128);
  writeCmd(0x2c);  // writeToRam()
  writeRepeatPixel(color, 128, 128);
}

void my_st7687s::drawPixel(int16_t x, int16_t y, uint16_t color) const {
  Serial.println("my_st7687s::drawPixel");
  // if(limitPixel(x, y) < 0) {return;}
  setCursorAddr(x, y, x, y);
  writeCmd(0x2c);  // writeToRam()
  uint8_t buf[2] = {static_cast<uint8_t>(color >> 8),
                    static_cast<uint8_t>(color)};
  writeDatBytes(buf, 2);
}
