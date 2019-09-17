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

DFRobot_ST7687S::DFRobot_ST7687S(uint8_t cs, uint8_t rs, uint8_t wr,
                                 uint8_t lck)
    : m_cs(cs), m_rs(rs), m_wr(wr), m_lck(lck) {
  ST7687S_SPIBEGIN(8000000);
  pinMode(m_cs, OUTPUT);
  digitalWrite(m_cs, HIGH);
  pinMode(m_rs, OUTPUT);
  digitalWrite(m_rs, HIGH);
  pinMode(m_lck, OUTPUT);
  digitalWrite(m_lck, HIGH);
  pinMode(m_wr, OUTPUT);
  digitalWrite(m_wr, HIGH);
}

void DFRobot_ST7687S::setCursorAddr(int16_t x0, int16_t y0, int16_t x1,
                                    int16_t y1) const {
  uint8_t addrBuf[2] = {(uint8_t)x0, (uint8_t)x1};
  writeCmd(0x2a);
  writeDatBytes(addrBuf, 2);
  addrBuf[0] = (uint8_t)y0;
  addrBuf[1] = (uint8_t)y1;
  writeCmd(0x2b);
  writeDatBytes(addrBuf, 2);
}

void DFRobot_ST7687S::fillScreen(uint16_t color) const {
  beforeDraw(0, 0, LCD_WIDTH, LCD_HEIGHT);
  for (uint16_t i = 0; i < LCD_WIDTH * LCD_HEIGHT; ++i) draw(color);
  afterDraw();
}

void DFRobot_ST7687S::beforeDraw(uint16_t x, uint16_t y, uint16_t w,
                                 uint16_t h) const {
  setCursorAddr(x, y, w, h);
  writeToRam();
#ifdef __ets__
  ESP.wdtFeed();
#endif
  digitalWrite(m_rs, HIGH);
  digitalWrite(m_cs, LOW);
}

void DFRobot_ST7687S::draw(uint8_t* p, uint16_t count) const {
  for (uint16_t i = 0; i < count; ++i) {
    SPI.transfer(p[i]);
    digitalWrite(m_lck, HIGH);
    digitalWrite(m_lck, LOW);
    digitalWrite(m_wr, LOW);
    digitalWrite(m_wr, HIGH);
  }
}

void DFRobot_ST7687S::draw(uint16_t color) const {
  uint8_t p[2] = {(uint8_t)(color >> 8), (uint8_t)color};
  draw(p, sizeof(p));
}

void DFRobot_ST7687S::afterDraw() const { digitalWrite(m_cs, HIGH); }

void DFRobot_ST7687S::begin(void) const {
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

void DFRobot_ST7687S::writeCmd(uint8_t cmd) const {
  digitalWrite(m_rs, LOW);
  digitalWrite(m_cs, LOW);

  SPI.transfer(cmd);
  digitalWrite(m_lck, HIGH);
  digitalWrite(m_lck, LOW);

  digitalWrite(m_wr, LOW);
  digitalWrite(m_wr, HIGH);
  digitalWrite(m_cs, HIGH);
}

void DFRobot_ST7687S::writeDat(uint8_t dat) const {
  writeDatBytes(&dat, sizeof(dat));
}

void DFRobot_ST7687S::writeDatBytes(uint8_t* pDat, uint16_t count) const {
#ifdef __ets__
  ESP.wdtFeed();
#endif
  digitalWrite(m_rs, HIGH);
  digitalWrite(m_cs, LOW);
  while (count--) {
    SPI.transfer(*pDat);
    digitalWrite(m_lck, HIGH);
    digitalWrite(m_lck, LOW);

    digitalWrite(m_wr, LOW);
    digitalWrite(m_wr, HIGH);
    pDat++;
  }
  digitalWrite(m_cs, HIGH);
}
