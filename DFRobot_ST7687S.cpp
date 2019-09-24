// ST7687S Datasheet
// https://www.displayfuture.com/Display/datasheet/controller/ST7687S.pdf

#include "DFRobot_ST7687S.h"
#include <Arduino.h>
#include <SPI.h>
#include <soc/gpio_struct.h>
#include <stdarg.h>
#include "DFRobot_ST7687S_Regs.h"

#define PIN_CS 0
#define PIN_RS 2
#define PIN_WR 4
#define PIN_LCK 15

// Macro
#if 0
#define SET_HIGH(pin) GPIO.out_w1ts = ((uint32_t)1 << pin)
#define SET_LOW(pin) GPIO.out_w1tc = ((uint32_t)1 << pin)
#define SET(pin, lev) \
  do {                \
    if (lev)          \
      SET_HIGH(pin);  \
    else              \
      SET_LOW(pin);   \
  } while (0)
#else
#define SET_HIGH(pin) GPIO_OUTPUT_SET(GPIO_ID_PIN(pin), 1)
#define SET_LOW(pin) GPIO_OUTPUT_SET(GPIO_ID_PIN(pin), 0)
#define SET(pin, lev) GPIO_OUTPUT_SET(GPIO_ID_PIN(pin), lev)
#endif

// __GNUC__ 、__GNUC_MINOR__ 、__GNUC_PATCHLEVEL__

#if ((__GNUC__ > 3) && (__GNUC_MINOR__ <= 3))
#define ST7687S_SPIBEGIN(x)               \
  do {                                    \
    SPI.begin();                          \
    SPI.setBitOrder(MSBFIRST);            \
    SPI.setClockDivider(SPI_CLOCK_DIV16); \
    SPI.setDataMode(SPI_MODE0);           \
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
void transfer(uint8_t const *p, uint16_t count) {
  while (count--) {
    SPI.transfer(*p);
    SET_HIGH(PIN_LCK);
    SET_LOW(PIN_LCK);
    SET_LOW(PIN_WR);
    SET_HIGH(PIN_WR);
    p++;
  }
}

void writeImpl(uint8_t const *p, uint16_t count, int lev) {
#ifdef __ets__
  ESP.wdtFeed();
#endif
  SET(PIN_RS, lev);
  SET_LOW(PIN_CS);
  transfer(p, count);
  SET_HIGH(PIN_CS);
}

void write(uint8_t cmd, uint8_t n = 0, ...) {
  uint8_t dat[16] = {0};
  va_list list;
  va_start(list, n);
  for (uint8_t i = 0; i < n; ++i) dat[i] = (uint8_t)va_arg(list, int);
  va_end(list);
  writeImpl(&cmd, sizeof(cmd), 0);
  writeImpl(dat, n, 1);
}
}  // namespace

void DFRobot_ST7687S::fillScreen(uint16_t color) const {
  beforeDraw(0, 0, LCD_WIDTH, LCD_HEIGHT);
  for (uint16_t i = 0; i < LCD_WIDTH * LCD_HEIGHT; ++i) draw(color);
  afterDraw();
}

void DFRobot_ST7687S::beforeDraw(uint8_t x, uint8_t y, uint8_t w,
                                 uint8_t h) const {
  // Column address set
  write(DF_CASET, 2, (uint8_t)x, (uint8_t)(x + w));
  // Row address set
  write(DF_RASET, 2, (uint8_t)y, (uint8_t)(y + h));
  // Memory write
  write(DF_RAMWR);
#ifdef __ets__
  ESP.wdtFeed();
#endif
  SET_HIGH(PIN_RS);
  SET_LOW(PIN_CS);
}

void DFRobot_ST7687S::draw(uint8_t const *p, uint16_t count) const {
  transfer(p, count);
}

void DFRobot_ST7687S::draw(uint16_t color) const {
  uint8_t p[2] = {(uint8_t)(color >> 8), (uint8_t)color};
  draw(p, sizeof(p));
}

void DFRobot_ST7687S::afterDraw() const { SET_HIGH(PIN_CS); }

void DFRobot_ST7687S::begin(void) const {
  ST7687S_SPIBEGIN(8000000);
  pinMode(PIN_CS, OUTPUT);
  pinMode(PIN_RS, OUTPUT);
  pinMode(PIN_WR, OUTPUT);
  pinMode(PIN_LCK, OUTPUT);
  SET_HIGH(PIN_CS);
  SET_HIGH(PIN_RS);
  SET_HIGH(PIN_WR);
  SET_HIGH(PIN_LCK);

  delay(120);

  // EEPROM data auto re-load control
  write(DF_AutoLoadSet, 1, 0x9F);
  // EEPROM control in
  write(DF_EEPCIN, 1, 0x00);
  delay(10);
  // EEPROM function selection
  write(DF_EEPANFSEL, 1, 0x01);
  delay(20);
  // Read from EEPROM
  write(DF_EEPRD);
  delay(20);
  // EEPROM control out
  write(DF_EEPCOUT);
  // Display off
  write(DF_DISPOFF);
  // Sleep out & booster on
  write(DF_SLPOUT);
  delay(30);
  // Vop setting
  // ctrL=1Bh 080416 5PCS 1Eh; 8PCS 2Ah
  write(DF_VopSet, 2, 0x17, 0x01);
  // Write contrast
  write(DF_WRCNTR, 1, 0x1E);
  // Bias selection
  write(DF_BiasSel, 1, 0x03);
  // Booster setting
  write(DF_BstBmpXSel, 1, 0x07);
  // ???
  write(0xC5, 1, 0x01);
  // FV3 with Booster x2 control
  write(DF_VgSorcSel, 1, 0x01);
  // Com/Seg Scan Direction for Glass layout
  write(DF_ComScanDir, 1, 0x00);
  // Analog circuit setting
  write(DF_ANASET, 1, 0x1D);
  // N-line control
  write(DF_NLInvSet, 1, 0x89);
  // Display Compensation Step
  write(DF_DispCompStep, 1, 0x02);
  // Frame Freq. in Temp range A,B,C and D
  write(DF_FRMSEL, 4, 0x07, 0x0C, 0x0C, 0x12);
  //
  write(DF_TEMPSEL, 8, 0x33, 0x33, 0x33, 0x00, 0x33, 0x66, 0x66, 0x66);
  // Display inversion off (normal)
  write(DF_INVOFF);
  // Column address set
  write(DF_CASET, 2, 0x00, 0x7F);
  // Row address set
  write(DF_RASET, 2, 0x00, 0x7F);
  // Interface pixel format
  write(DF_COLMOD, 1, 0x05);
  // Memory data access control
  write(DF_MADCTR, 1, 0x80);
  // Display Duty setting
  write(DF_DutySet, 1, 0x7F);
  // Display on
  write(DF_DISPON);
  // Set Frame RGB value
  write(DF_FrameSet, 16, 0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E, 0x10,
        0x12, 0x14, 0x16, 0x18, 0x1A, 0x1C, 0x1E);
  // Display on
  write(DF_DISPON);
}
