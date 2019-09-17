#include "DFRobot_ST7687S.h"
#include <SPI.h>

// __GNUC__ 、__GNUC_MINOR__ 、__GNUC_PATCHLEVEL__

#if ((__GNUC__ > 3) && (__GNUC_MINOR__ <= 3))
#define ST7687S_SPIBEGIN(x) SPI.begin()
#define ST7687S_SPIEND() SPI.end()
#else
#define ST7687S_SPIBEGIN(x) \
  SPI.begin();              \
  SPI.beginTransaction(SPISettings(x, MSBFIRST, SPI_MODE0))
#define ST7687S_SPIEND() SPI.endTransaction()
#endif

DFRobot_ST7687S::DFRobot_ST7687S(uint8_t m_cs_, uint8_t m_cd_, uint8_t m_wr_,
                                 uint8_t m_rck_)
    : m_cs(m_cs_), m_cd(m_cd_), m_wr(m_wr_), m_rck(m_rck_) {
  ST7687S_SPIBEGIN(4000000);
  pinMode(m_cs, OUTPUT);
  digitalWrite(m_cs, HIGH);
  pinMode(m_cd, OUTPUT);
  digitalWrite(m_cd, HIGH);
  pinMode(m_rck, OUTPUT);
  digitalWrite(m_rck, HIGH);
  pinMode(m_wr, OUTPUT);
  digitalWrite(m_wr, HIGH);
}

void DFRobot_ST7687S::setCursorAddr(int16_t x0, int16_t y0, int16_t x1,
                                    int16_t y1) {
  uint8_t addrBuf[2] = {(uint8_t)x0, (uint8_t)x1};
  writeCmd(0x2a);
  writeDatBytes(addrBuf, 2);
  addrBuf[0] = (uint8_t)y0;
  addrBuf[1] = (uint8_t)y1;
  writeCmd(0x2b);
  writeDatBytes(addrBuf, 2);
}

void DFRobot_ST7687S::fillScreen(uint16_t color) {
  setCursorAddr(0, 0, 128, 128);
  writeToRam();
  writeRepeatPixel(color, 128, 128);
}

int16_t DFRobot_ST7687S::begin(void) {
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
  return 0;
}

void DFRobot_ST7687S::drawPixel(int16_t x, int16_t y, uint16_t color) {
  uint8_t colorBuf[2] = {(uint8_t)(color >> 8), (uint8_t)color};
  // if (limitPixel(x, y) < 0) return;
  setCursorAddr(x, y, x, y);
  writeToRam();
  writeDatBytes(colorBuf, 2);
}

void DFRobot_ST7687S::writeCmd(uint8_t cmd) {
  digitalWrite(m_cd, LOW);
  digitalWrite(m_cs, LOW);

  SPI.transfer(cmd);
  digitalWrite(m_rck, HIGH);
  digitalWrite(m_rck, LOW);

  digitalWrite(m_wr, LOW);
  digitalWrite(m_wr, HIGH);
  digitalWrite(m_cs, HIGH);
}

void DFRobot_ST7687S::writeDat(uint8_t dat) {
  digitalWrite(m_cd, HIGH);
  digitalWrite(m_cs, LOW);

  SPI.transfer(dat);
  digitalWrite(m_rck, HIGH);
  digitalWrite(m_rck, LOW);

  digitalWrite(m_wr, LOW);
  digitalWrite(m_wr, HIGH);
  digitalWrite(m_cs, HIGH);
}

void DFRobot_ST7687S::writeDatBytes(uint8_t* pDat, uint16_t count) {
#ifdef __ets__
  ESP.wdtFeed();
#endif
  digitalWrite(m_cd, HIGH);
  digitalWrite(m_cs, LOW);
  while (count--) {
    SPI.transfer(*pDat);
    digitalWrite(m_rck, HIGH);
    digitalWrite(m_rck, LOW);

    digitalWrite(m_wr, LOW);
    digitalWrite(m_wr, HIGH);
    pDat++;
  }
  digitalWrite(m_cs, HIGH);
}

void DFRobot_ST7687S::writeRepeatPixel(uint16_t color, uint16_t count,
                                       uint16_t repeatCount) {
  uint8_t buf[2] = {(uint8_t)(color >> 8), (uint8_t)color};
  for (uint32_t i = 0; i < repeatCount * count; i++) writeDatBytes(buf, 2);
}
