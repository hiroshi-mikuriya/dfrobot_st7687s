// ST7687S Datasheet
// https://www.displayfuture.com/Display/datasheet/controller/ST7687S.pdf

#include "DFRobot_ST7687S.h"
#include <SPI.h>
#include <stdarg.h>

// CS
#define DDR_CS DDRD
#define PORT_CS PORTD
#define PIN_CS PD3

// RS
#define DDR_RS DDRD
#define PORT_RS PORTD
#define PIN_RS PD5

// WR
#define DDR_WR DDRD
#define PORT_WR PORTD
#define PIN_WR PD6

// LCK
#define DDR_LCK DDRD
#define PORT_LCK PORTD
#define PIN_LCK PD7

// Macro
#define SET_HIGH(port, pin) port |= _BV(pin)
#define SET_LOW(port, pin) port &= ~_BV(pin)

// __GNUC__ 、__GNUC_MINOR__ 、__GNUC_PATCHLEVEL__

#if ((__GNUC__ > 3) && (__GNUC_MINOR__ <= 3))
#define ST7687S_SPIBEGIN(x)              \
  do {                                   \
    SPI.begin();                         \
    SPI.setBitOrder(MSBFIRST);           \
    SPI.setClockDivider(SPI_CLOCK_DIV4); \
    SPI.setDataMode(SPI_MODE0);          \
  } while (0)
#define ST7687S_SPIEND() SPI.end()
#else
#define ST7687S_SPIBEGIN(x)                                    \
  do {                                                         \
    SPI.begin();                                               \
    SPI.beginTransaction(SPISettings(x, MSBFIRST, SPI_MODE0)); \
  } while (0)
#define ST7687S_SPIEND() SPI.endTransaction()
#endif

namespace {
void writeCmd(uint8_t cmd) {
  SET_LOW(PORT_RS, PIN_RS);
  SET_LOW(PORT_CS, PIN_CS);

  SPI.transfer(cmd);
  SET_HIGH(PORT_LCK, PIN_LCK);
  SET_LOW(PORT_LCK, PIN_LCK);

  SET_LOW(PORT_WR, PIN_WR);
  SET_HIGH(PORT_WR, PIN_WR);
  SET_HIGH(PORT_CS, PIN_CS);
}

void writeDat(uint8_t* pDat, uint16_t count) {
#ifdef __ets__
  ESP.wdtFeed();
#endif
  SET_HIGH(PORT_RS, PIN_RS);
  SET_LOW(PORT_CS, PIN_CS);
  while (count--) {
    SPI.transfer(*pDat);
    SET_HIGH(PORT_LCK, PIN_LCK);
    SET_LOW(PORT_LCK, PIN_LCK);

    SET_LOW(PORT_WR, PIN_WR);
    SET_HIGH(PORT_WR, PIN_WR);
    pDat++;
  }
  SET_HIGH(PORT_CS, PIN_CS);
}

void write(uint8_t cmd, uint8_t n = 0, ...) {
  uint8_t buf[16] = {0};
  va_list list;
  va_start(list, n);
  for (uint8_t i = 0; i < n; ++i) buf[i] = (uint8_t)va_arg(list, int);
  va_end(list);
  writeCmd(cmd);
  writeDat(buf, n);
}

void setCursorAddr(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
  // CASET Column address set
  write(0x2a, 2, (uint8_t)x0, (uint8_t)x1);
  // RASET Row address set
  write(0x2b, 2, (uint8_t)y0, (uint8_t)y1);
}
}  // namespace

void DFRobot_ST7687S::fillScreen(uint16_t color) const {
  beforeDraw(0, 0, LCD_WIDTH, LCD_HEIGHT);
  for (uint16_t i = 0; i < LCD_WIDTH * LCD_HEIGHT; ++i) draw(color);
  afterDraw();
}

void DFRobot_ST7687S::beforeDraw(uint16_t x, uint16_t y, uint16_t w,
                                 uint16_t h) const {
  setCursorAddr(x, y, w, h);
  // RAMWR Memory write
  write(0x2c);
#ifdef __ets__
  ESP.wdtFeed();
#endif
  SET_HIGH(PORT_RS, PIN_RS);
  SET_LOW(PORT_CS, PIN_CS);
}

void DFRobot_ST7687S::draw(uint8_t* p, uint16_t count) const {
  for (uint16_t i = 0; i < count; ++i) {
    SPI.transfer(p[i]);
    SET_HIGH(PORT_LCK, PIN_LCK);
    SET_LOW(PORT_LCK, PIN_LCK);
    SET_LOW(PORT_WR, PIN_WR);
    SET_HIGH(PORT_WR, PIN_WR);
  }
}

void DFRobot_ST7687S::draw(uint16_t color) const {
  uint8_t p[2] = {(uint8_t)(color >> 8), (uint8_t)color};
  draw(p, sizeof(p));
}

void DFRobot_ST7687S::afterDraw() const { SET_HIGH(PORT_CS, PIN_CS); }

void DFRobot_ST7687S::begin(void) const {
  ST7687S_SPIBEGIN(4000000);
  DDR_CS |= _BV(PIN_CS);
  DDR_RS |= _BV(PIN_RS);
  DDR_WR |= _BV(PIN_WR);
  DDR_LCK |= _BV(PIN_LCK);
  SET_HIGH(PORT_CS, PIN_CS);
  SET_HIGH(PORT_RS, PIN_RS);
  SET_HIGH(PORT_WR, PIN_WR);
  SET_HIGH(PORT_LCK, PIN_LCK);

  delay(120);

  // AutoLoadSet EEPROM data auto re-load control
  write(0xD7, 1, 0x9F);

  // EEPCIN EEPROM control in
  write(0xE0, 1, 0x00);
  delay(10);

  // EEPANFSEL EEPROM function selection
  write(0xFA, 1, 0x01);
  delay(20);

  // EEPRD Read from EEPROM
  write(0xE3);
  delay(20);

  // EEPCOUT EEPROM control out
  write(0xE1);

  // DISPOFF Display off
  write(0x28);

  // SLPOUT Sleep out & booster on
  write(0x11);
  delay(30);

  // VopSet Vop setting
  // ctrL=1Bh 080416 5PCS 1Eh; 8PCS 2Ah
  write(0xC0, 2, 0x17, 0x01);

  // WRCNTR Write contrast
  write(0x25, 1, 0x1E);

  // BiasSel Bias selection
  write(0xC3, 1, 0x03);

  // BstBmpXSel Booster setting
  write(0xC4, 1, 0x07);

  // ???
  write(0xC5, 1, 0x01);

  // VgSorcSel FV3 with Booster x2 control
  write(0xCB, 1, 0x01);

  // ComScanDir Com/Seg Scan Direction for Glass layout
  write(0xB7, 1, 0x00);

  // ANASET Analog circuit setting
  write(0xD0, 1, 0x1D);

  // NLInvSet N-line control
  write(0xB5, 1, 0x89);

  // DispCompStep Display Compensation Step
  write(0xBD, 1, 0x02);

  // FRMSEL Frame Freq. in Temp range A,B,C and D
  write(0xF0, 4, 0x07, 0x0C, 0x0C, 0x12);

  // TEMPSEL
  write(0xF4, 8, 0x33, 0x33, 0x33, 0x00, 0x33, 0x66, 0x66, 0x66);

  // INVOFF Display inversion off (normal)
  write(0x20);

  // CASET Column address set
  write(0x2A, 2, 0x00, 0x7F);

  // RASET Row address set
  write(0x2B, 2, 0x00, 0x7F);

  // COLMOD Interface pixel format
  write(0x3A, 1, 0x05);

  // MADCTR Memory data access control
  write(0x36, 1, 0x80);

  // DutySet Display Duty setting
  write(0xB0, 1, 0x7F);

  // DISPON Display on
  write(0x29);

  // FrameSet Set Frame RGB value
  write(0xF9, 16, 0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E, 0x10, 0x12,
        0x14, 0x16, 0x18, 0x1A, 0x1C, 0x1E);

  // DISPON Display on
  write(0x29);
}
