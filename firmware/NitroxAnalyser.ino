#include <Wire.h>
#include <OzOLED.h>

#define LEFT_BUTTON_PIN     4
#define RIGHT_BUTTON_PIN    5


#define O2_CELL_ANALOG        A0
#define BATTERY_ANALOG        A1
#define FLOW_DETECTOR_ANALOG  A2

const byte welcomeScreen[] PROGMEM = {0X00, 0X00, 0X00, 0X00, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0XC0, 0XA0, 0XA0, 0XC0, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0XF0, 0XF0, 0X80, 0X00, 0XF0, 0XF0, 0X00, 0X00, 0XF0, 0XF0, 0X10, 0X10, 0XF0, 0XF0, 0X10, 0X10, 0X00, 0XF0, 0XF0, 0X10, 0X10, 0XF0, 0XF0, 0X00, 0XF0, 0XF0, 0X10, 0X10, 0XF0, 0XF0, 0X00, 0X00, 0X70, 0XF0, 0X80, 0XE0, 0X70, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0XE0, 0XF0, 0XF0, 0X00, 0X00, 0X00, 0XF0, 0XF0, 0X80, 0X00, 0XF0, 0XF0, 0X00, 0X00, 0X00, 0XE0, 0XF0, 0XF0, 0X00, 0X00, 0X00, 0XF0, 0XF0, 0X00, 0X00, 0X00, 0X10, 0XF0, 0XE0, 0X00, 0XC0, 0XF0, 0X10, 0X00, 0XF0, 0XF0, 0X10, 0X10, 0XF0, 0X70, 0X00, 0XF0, 0XF0, 0X10, 0X10, 0X10, 0X00, 0XF0, 0XF0, 0X10, 0X10, 0XF0, 0XF0, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
                                      0X00, 0X00, 0X00, 0X00, 0X0F, 0X0A, 0X8A, 0X48, 0X6F, 0X7F, 0X5D, 0XD8, 0X98, 0X5D, 0X77, 0X40, 0X40, 0X80, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0XFF, 0XFF, 0X07, 0X3E, 0XFF, 0XFF, 0X00, 0X00, 0XFF, 0XFF, 0X00, 0X00, 0XFF, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0XFF, 0X0E, 0X3E, 0XF3, 0X83, 0X00, 0XFF, 0XFF, 0XC0, 0XC0, 0XFF, 0XFF, 0X00, 0X80, 0XF0, 0X7F, 0X0F, 0X7F, 0XF0, 0X80, 0X00, 0X00, 0X00, 0X80, 0XFC, 0X3F, 0X30, 0X3F, 0XFE, 0XC0, 0X00, 0XFF, 0XFF, 0X07, 0X3E, 0XFF, 0XFF, 0X00, 0X80, 0XFC, 0X3F, 0X30, 0X3F, 0XFE, 0XC0, 0X00, 0XFF, 0XFF, 0XC0, 0XC0, 0X80, 0X00, 0X00, 0X07, 0XFF, 0X0F, 0X00, 0X00, 0X00, 0XF1, 0XF3, 0XC7, 0XCE, 0XFC, 0X70, 0X00, 0XFF, 0XFF, 0XC6, 0XC6, 0XC0, 0X00, 0XFF, 0XFF, 0X0E, 0X3E, 0XF3, 0X83, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
                                      0X00, 0X00, 0X00, 0X00, 0XFE, 0X03, 0XF0, 0X00, 0XFC, 0X00, 0X00, 0X24, 0XA4, 0X98, 0X98, 0XAC, 0X24, 0X00, 0X03, 0XFC, 0X00, 0X00, 0X00, 0X00, 0X00, 0XF0, 0X00, 0X00, 0XE0, 0X70, 0X00, 0X00, 0X00, 0X00, 0XF0, 0XE0, 0X00, 0X00, 0X00, 0X00, 0XF0, 0X30, 0X30, 0XF0, 0XE0, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
                                      0X00, 0X00, 0X00, 0X00, 0XFF, 0X00, 0XFF, 0X00, 0XFF, 0X00, 0X00, 0XC7, 0X28, 0X28, 0XC8, 0X88, 0X47, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0X00, 0X00, 0X07, 0X7F, 0X7C, 0X0F, 0X00, 0X00, 0X00, 0X00, 0X00, 0X7F, 0X7F, 0X00, 0X60, 0X00, 0X00, 0X7F, 0X60, 0X40, 0X7F, 0X7F, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
                                      0X00, 0X00, 0X00, 0X00, 0XFF, 0X00, 0XFF, 0X00, 0XFF, 0X00, 0X00, 0XFB, 0X22, 0X22, 0X23, 0X22, 0X22, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
                                      0X00, 0X00, 0X00, 0X00, 0XFF, 0X00, 0XFF, 0X00, 0XFF, 0X00, 0X00, 0X8A, 0XCA, 0X4A, 0X2A, 0X3A, 0X1A, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
                                      0X00, 0X00, 0X00, 0X00, 0XFF, 0X02, 0X06, 0X04, 0X05, 0X04, 0X04, 0X05, 0X05, 0X05, 0X05, 0X05, 0X05, 0X04, 0X02, 0XFF, 0X00, 0X00, 0X00, 0X00, 0XFE, 0X02, 0X02, 0X8E, 0X00, 0XE0, 0XF0, 0X10, 0X70, 0XE0, 0XF0, 0X50, 0X70, 0X00, 0X80, 0XF0, 0X50, 0XF0, 0X00, 0X10, 0XFC, 0X10, 0XE0, 0XF0, 0X50, 0X70, 0X00, 0XE0, 0XF0, 0X10, 0XFE, 0X00, 0X00, 0X00, 0XFE, 0XFE, 0X10, 0XF0, 0X00, 0X10, 0XF0, 0X80, 0XF0, 0X00, 0X00, 0X00, 0X00, 0X9E, 0X32, 0X62, 0XCE, 0XE0, 0XF0, 0X50, 0X70, 0X00, 0XE0, 0XF0, 0X10, 0X70, 0XE0, 0XF0, 0X10, 0XF0, 0X00, 0X04, 0XF4, 0XE0, 0XF0, 0X10, 0XF0, 0X00, 0X00, 0X00, 0X00, 0XF8, 0X8E, 0XFE, 0XC0, 0X00, 0XFE, 0X10, 0XF0, 0X80, 0XF0, 0X00, 0XE0, 0XF0, 0X50, 0X70, 0X00, 0X20, 0X70, 0XD0, 0XB0, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
                                      0X00, 0X00, 0X00, 0X00, 0X01, 0X01, 0X02, 0X02, 0X04, 0X04, 0X04, 0X04, 0X04, 0X04, 0X04, 0X04, 0X02, 0X02, 0X03, 0X01, 0X00, 0X00, 0X00, 0X00, 0X03, 0X02, 0X02, 0X03, 0X00, 0X03, 0X03, 0X00, 0X00, 0X01, 0X03, 0X02, 0X03, 0X00, 0X01, 0X03, 0X02, 0X03, 0X00, 0X00, 0X03, 0X02, 0X01, 0X03, 0X02, 0X03, 0X00, 0X01, 0X03, 0X02, 0X03, 0X00, 0X00, 0X00, 0X03, 0X03, 0X02, 0X03, 0X00, 0X00, 0X09, 0X0F, 0X00, 0X00, 0X00, 0X00, 0X00, 0X03, 0X02, 0X03, 0X03, 0X01, 0X03, 0X02, 0X03, 0X00, 0X03, 0X03, 0X00, 0X00, 0X01, 0X0B, 0X0A, 0X0F, 0X00, 0X00, 0X03, 0X01, 0X03, 0X02, 0X03, 0X00, 0X00, 0X00, 0X02, 0X03, 0X00, 0X01, 0X03, 0X02, 0X03, 0X00, 0X01, 0X03, 0X00, 0X00, 0X01, 0X03, 0X02, 0X03, 0X00, 0X01, 0X03, 0X02, 0X03, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
                                     };

const byte digit_0[] PROGMEM = {0x0C, 0x20, 0x00, 0x80, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x80, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x01, 0x01, 0x01, 0x01, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x03, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x00};
const byte digit_1[] PROGMEM = {0x05, 0x20, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x0E, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x07, 0x07, 0x07, 0x00};
const byte digit_2[] PROGMEM = {0x0C, 0x20, 0x00, 0x80, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x80, 0x00, 0x00, 0x0F, 0x0F, 0x0F, 0x01, 0x01, 0x81, 0xC1, 0xFF, 0xFF, 0x3F, 0x00, 0x00, 0x00, 0xC0, 0xE0, 0xF8, 0x7E, 0x1F, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x00};
const byte digit_3[] PROGMEM = {0x0C, 0x20, 0x00, 0x80, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x80, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x01, 0x81, 0x81, 0xC1, 0xFF, 0x7F, 0x3F, 0x00, 0x00, 0xF0, 0xF0, 0xF0, 0x01, 0x03, 0x03, 0x07, 0xFE, 0xFC, 0xF8, 0x00, 0x00, 0x03, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x00};
const byte digit_4[] PROGMEM = {0x0C, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xF8, 0xFF, 0x3F, 0x07, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x70, 0x7E, 0x7F, 0x7F, 0x71, 0x70, 0x70, 0xFF, 0xFF, 0xFF, 0x70, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0x07, 0x00, 0x00};
const byte digit_5[] PROGMEM = {0x0C, 0x20, 0x00, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xC1, 0xC1, 0xC1, 0xC1, 0xC1, 0xC1, 0x81, 0x00, 0x00, 0xE1, 0xE1, 0xE1, 0x01, 0x01, 0x01, 0x01, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x03, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x00};
const byte digit_6[] PROGMEM = {0x0C, 0x20, 0x00, 0x80, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x80, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x81, 0x81, 0x81, 0x81, 0x8F, 0x8F, 0x0F, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x03, 0x03, 0x03, 0x03, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x03, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x00};
const byte digit_7[] PROGMEM = {0x0C, 0x20, 0x00, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x0F, 0x0F, 0x0F, 0x01, 0x01, 0xC1, 0xF9, 0xFF, 0x3F, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xFC, 0xFF, 0x1F, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x07, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
const byte digit_8[] PROGMEM = {0x0C, 0x20, 0x00, 0x80, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x80, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0xC1, 0x81, 0x81, 0xC1, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0xFC, 0xFE, 0xFF, 0x03, 0x03, 0x03, 0x03, 0xFF, 0xFE, 0xFC, 0x00, 0x00, 0x03, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x00};
const byte digit_9[] PROGMEM = {0x0C, 0x20, 0x00, 0x80, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x80, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x81, 0x81, 0x81, 0x81, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xE1, 0xE3, 0xE3, 0x03, 0x03, 0x03, 0x03, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x03, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x00};
const byte digit_point[] PROGMEM = {0x05, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0x00, 0x07, 0x07, 0x07, 0x00};
const byte digit_blank[] PROGMEM = {0x0C, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const byte digit_percent[] PROGMEM = {0x11, 0x20, 0x00, 0x80, 0xC0, 0xC0, 0xC0, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x80, 0x80, 0xFF, 0xFF, 0x00, 0xC0, 0xF0, 0x3E, 0x0F, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x81, 0xE1, 0xF8, 0x1F, 0x07, 0x01, 0xFE, 0xFF, 0x03, 0x03, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x06, 0x07, 0x03, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x06, 0x06, 0x07, 0x03, 0x00};
const byte digit_o2[] PROGMEM = {0x12, 0x20, 0x00, 0x80, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x01, 0x01, 0x01, 0x01, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x18, 0x9C, 0xCC, 0x7C, 0x18, 0x00, 0x00, 0x03, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x00, 0x06, 0x07, 0x06, 0x06, 0x06, 0x00 };
const byte * digits[] = {&digit_0[0], &digit_1[0], &digit_2[0], &digit_3[0], &digit_4[0], &digit_5[0], &digit_6[0], &digit_7[0], &digit_8[0], &digit_9[0], &digit_point[0], &digit_blank[0], &digit_percent[0], &digit_o2[0]};

const byte startMenu[] PROGMEM = {0x1D, 0x10, 0x00, 0x70, 0x88, 0x88, 0x88, 0x10, 0x00, 0x20, 0xF8, 0x20, 0x20, 0x00, 0x00, 0x40, 0x20, 0x20, 0x20, 0xC0, 0x00, 0x00, 0xE0, 0x40, 0x20, 0x20, 0x20, 0xF8, 0x20, 0x20, 0x00, 0x00, 0x04, 0x08, 0x08, 0x08, 0x07, 0x00, 0x00, 0x07, 0x08, 0x08, 0x00, 0x00, 0x06, 0x09, 0x09, 0x05, 0x0F, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x07, 0x08, 0x08, 0x00};
const byte calibMenu[] PROGMEM = {0x1F, 0x10, 0x00, 0xE0, 0x10, 0x08, 0x08, 0x08, 0x08, 0x10, 0x00, 0x40, 0x20, 0x20, 0x20, 0xC0, 0x00, 0x00, 0xFC, 0x00, 0x00, 0x00, 0xE8, 0x00, 0x00, 0x00, 0xFC, 0x40, 0x20, 0x20, 0x20, 0xC0, 0x00, 0x00, 0x03, 0x04, 0x08, 0x08, 0x08, 0x08, 0x04, 0x00, 0x06, 0x09, 0x09, 0x05, 0x0F, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x0F, 0x04, 0x08, 0x08, 0x08, 0x07, 0x00};
const byte bat100Percent[] PROGMEM = {0x10, 0x10, 0xF0, 0x10, 0xD0, 0xD0, 0x10, 0xD0, 0xD0, 0x10, 0xD0, 0xD0, 0x10, 0xD0, 0xD0, 0x10, 0xE0, 0x80, 0x0F, 0x08, 0x0B, 0x0B, 0x08, 0x0B, 0x0B, 0x08, 0x0B, 0x0B, 0x08, 0x0B, 0x0B, 0x08, 0x07, 0x01};
const byte bat75Percent[] PROGMEM = {0x10, 0x10, 0xF0, 0x10, 0xD0, 0xD0, 0x10, 0xD0, 0xD0, 0x10, 0xD0, 0xD0, 0x10, 0x90, 0x50, 0x10, 0xE0, 0x80, 0x0F, 0x08, 0x0B, 0x0B, 0x08, 0x0B, 0x0B, 0x08, 0x0B, 0x0B, 0x08, 0x0A, 0x09, 0x08, 0x07, 0x01};
const byte bat50Percent[] PROGMEM = {0x10, 0x10, 0xF0, 0x10, 0xD0, 0xD0, 0x10, 0xD0, 0xD0, 0x10, 0x90, 0x50, 0x10, 0x90, 0x50, 0x10, 0xE0, 0x80, 0x0F, 0x08, 0x0B, 0x0B, 0x08, 0x0B, 0x0B, 0x08, 0x0A, 0x09, 0x08, 0x0A, 0x09, 0x08, 0x07, 0x01};
const byte bat25Percent[] PROGMEM = {0x10, 0x10, 0xF0, 0x10, 0xD0, 0xD0, 0x10, 0x90, 0x50, 0x10, 0x90, 0x50, 0x10, 0x90, 0x50, 0x10, 0xE0, 0x80, 0x0F, 0x08, 0x0B, 0x0B, 0x08, 0x0A, 0x09, 0x08, 0x0A, 0x09, 0x08, 0x0A, 0x09, 0x08, 0x07, 0x01};
const byte bat0Percent[] PROGMEM = {0x10, 0x10, 0xF0, 0x10, 0x90, 0x50, 0x10, 0x90, 0x50, 0x10, 0x90, 0x50, 0x10, 0x90, 0x50, 0x10, 0xE0, 0x80, 0x0F, 0x08, 0x0A, 0x09, 0x08, 0x0A, 0x09, 0x08, 0x0A, 0x09, 0x08, 0x0A, 0x09, 0x08, 0x07, 0x01};
const byte progressElement[] PROGMEM = {0x04, 0x08, 0x7E, 0x7E, 0x7E, 0x00};

const byte center[] PROGMEM ={0x02, 0x08, 0xFE, 0xFE};
const byte left[] PROGMEM ={0x05, 0x08, 0x10, 0x28, 0x44, 0x82, 0x00};
const byte right[] PROGMEM ={0x05, 0x08, 0x00, 0x82, 0x44, 0x28, 0x10};

int state = 0;
unsigned long start = 0;
int analogValue = 0;
long decval = 0;
byte digs[8] ;
boolean numDigitFound = false;
byte progress = 0, oldprogress = 0;
byte gencnt = 0;
int decvalcp = decval;
int pos = 0;
int index = 1;
byte buttonsOldValues[] = {1, 1};
unsigned long buttonLastStateChangeMillis[] = {millis(), millis()};
int anaSteps=0;
int blinkcnt=0;
long mappedValue = 0;
int calSteps=0;

/********************************************************************************************************************************************/
/**
 * The arduino setup method
 */
void setup() {
  Serial.begin(115200);
  OzOled.init();                       // initialze Oscar OLED display
  OzOled.sendCommand(0xA0);
  delay(100);
  state = 0;

  pinMode(LEFT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON_PIN, INPUT_PULLUP);
}

/**
 * Clears display (override OzOled function by clearing by column and not by character)
 */
void clearDisplay(int spage, int epage, int scol, int ecol)  {
  byte mp = 0, column = 0;

  for (mp = spage; mp < epage; mp++) {

    OzOled.sendCommand(0x00 + (scol & 0x0F));     //set column lower address
    OzOled.sendCommand(0x10 + ((scol >> 4) & 0x0F)); //set column higher address
    OzOled.sendCommand(0xB0 + mp);//set page address

    for (column = scol; column < ecol; column++) { //clear all columns
      OzOled.sendData(0);
    }
  }

  OzOled.sendCommand(0x00 + (76 & 0x0F));     //set column lower address
  OzOled.sendCommand(0x10 + ((76 >> 4) & 0x0F)); //set column higher address
  OzOled.sendCommand(0xB0 + mp);//set page address
}

/**
 * Sets netxt state to analyze state
 */
void actionOnStartButtonClick() {
  Serial.println(F("Start analyse button pressed"));
  state = 3;
}

/**
 * Sets next main state to calibration state
 */
void actionOnCalibrationButtonClick() {
  Serial.println(F("Start Calibration button pressed"));
  state = 4;
}

/**
 * Checks button and debounce
 */
void checkAndDebounce(int index, int pinId, void (*f)()) {
  if (buttonsOldValues[index] != digitalRead(pinId) && (millis() - buttonLastStateChangeMillis[index]) > 100) {
    buttonLastStateChangeMillis[index] = millis();

    //Change occured let's handle
    if (buttonsOldValues[index] == 1) {
      //Falling edge => must do job
      f();
    }

    buttonsOldValues[index] = digitalRead(pinId);
  }
}

/**
 * Displays the splash screen and move to next state (delay before display the second screen)
 */
void displaySplashScreen() {
  Serial.println(F("Displaying splash Screen"));
  OzOled.clearDisplay();               // clear the screen and set start position to top left corner
  OzOled.drawBitmap(welcomeScreen, 2, 0, 128, 64);
  Serial.println(F("Changing state : 0 -> 1"));
  state = 1;
  start = millis();  
}

/**
 * A non blocking delay. 
 * if delay elapsed returns true else false
 */
boolean nonBlockingDelayElapsed(int ms) {
  if ((millis() - start) > ms) {
    return true;
  }
  return false;
}

/**
 * Displays the small battery status by reading the battery remaining voltage.
 * Battery status moves from full -> 75% -> 50% -> 25% ->nearly empty 
 */
void displayBatteryStatus() {
  switch (map(analogRead(BATTERY_ANALOG), 800, 1023, 0, 4)) {
    default:
    case 0:
      OzOled.drawBitmap(&bat0Percent[2], 56, 48, (int)pgm_read_byte(&bat0Percent[0]), (int)pgm_read_byte(&bat0Percent[1]));
      break;
    case 1:
      OzOled.drawBitmap(&bat25Percent[2], 56, 48, (int)pgm_read_byte(&bat25Percent[0]), (int)pgm_read_byte(&bat25Percent[1]));
      break;
    case 2:
      OzOled.drawBitmap(&bat50Percent[2], 56, 48, (int)pgm_read_byte(&bat50Percent[0]), (int)pgm_read_byte(&bat50Percent[1]));
      break;
    case 3:
      OzOled.drawBitmap(&bat75Percent[2], 56, 48, (int)pgm_read_byte(&bat75Percent[0]), (int)pgm_read_byte(&bat75Percent[1]));
      break;
    case 4:
      OzOled.drawBitmap(&bat100Percent[2], 56, 48, (int)pgm_read_byte(&bat100Percent[0]), (int)pgm_read_byte(&bat100Percent[1]));
      break;
  }  
}

/**
 * Displays the main interface screen and after that set state to idle  aka state = 1000
 */
void displayMainScreen() {
  Serial.println(F("Displaying main interface"));
  OzOled.clearDisplay();               // clear the screen and set start position to top left corner
  OzOled.drawBitmap(&startMenu[2], 8, 48, (int)pgm_read_byte(&startMenu[0]), (int)pgm_read_byte(&startMenu[1]));
  OzOled.drawBitmap(&calibMenu[2], 88, 48, (int)pgm_read_byte(&calibMenu[0]), (int)pgm_read_byte(&calibMenu[1]));
  displayBatteryStatus();
  Serial.println(F("Changing state : 2 -> idle"));
  state = 1000;
}

/**
 * Does flow exists ? 
 */
void analyzeTaskDoesFlowExists() {
  //read air circulation sensor (motor with propeler)
  if(analogRead(FLOW_DETECTOR_ANALOG) >  50) {
    //flow exists
    anaSteps = 3;
    Serial.println(F("Analyzer step 0 -> 3"));
  } else {
    //flow doesn't exist
    anaSteps = 1;
    Serial.println(F("Analyzer step 0 -> 1"));        
  }        
}

/**
 * Prints some text
 */
void printEnableNitroxFlow() {
  clearDisplay(0, 4, 0, 128);
  OzOled.printString("Enable nitrox", 1, 0, 13);
  OzOled.printString("flow", 6, 1, 4);  
}

/**
 * Displays the information message
 */
void analyzeTaskDisplayMessageToEnableNitroxFlow() {
  printEnableNitroxFlow();
  start = millis();
  anaSteps = 2;
  blinkcnt = 0;
  Serial.println(F("Analyzer step 1 -> 2"));   
}

/**
 * Blinks the information message and clear the screen at the end
 */
void analyzeTaskBlinkingText() {
  switch(blinkcnt) {
    case 0: //wait 1.5s and clear
      if(millis() - start > 1500) {
        clearDisplay(0,2,0,128);                              
        start = millis();
        blinkcnt++;
      }        
      break;
    case 1: //wait 0.5s and display
    case 3:
    case 5:
      if(millis()-start > 500) {
        printEnableNitroxFlow();
        start = millis();
        blinkcnt++;
      }
      break;
    case 2: //wait 0.5 and clear
    case 4:
    case 6:  
      if(millis() - start > 500) {
        clearDisplay(0,2,0,128);                              
        start = millis();
        blinkcnt++;
      }    
      break;
    case 7:                                 
      if(millis() - start > 500) {
        anaSteps = 0; //Try to detect flow again
        Serial.println(F("Analyzer step 2 -> 0"));
      }
      break;
  }  
}

/**
 * The math.sign function
 */
byte sign(int value) {
  if(value>0)
    return 1;
  else if (value < 0)
    return -1;
  else
    return 0;
}

/**
 * Checks the flow and display a pointer to help flow regulation before analyze
 */
void analyzeTaskFlowRegulation() {
  if(millis() - start > 500) {
    //check each 500ms 
    start = millis();
  
    //flow between 200 and 250 is perfect  
    mappedValue = map(analogRead(FLOW_DETECTOR_ANALOG), 0, 1023, 11, -11);
    
    Serial.print(F("Mapped Value : "));
    Serial.println(mappedValue);
  
    //Clears display
    clearDisplay(4, 5, 0, 128);
    
    //Draws the center
    OzOled.drawBitmap(&center[2], 63, 32, (int)pgm_read_byte(&center[0]), (int)pgm_read_byte(&center[1]));
    
    //draws the indicator for the flow
  
    if(mappedValue < 0 ){
      for(int i = mappedValue; i < 0; i++) {
        OzOled.drawBitmap(&left[2], 63 + i* (int)pgm_read_byte(&left[0]), 32, (int)pgm_read_byte(&left[0]), (int)pgm_read_byte(&left[1]));
      }
    }else if (mappedValue > 0) {
      for(int i=0; i < mappedValue; i++) {
        OzOled.drawBitmap(&right[2], 65 + i * (int)pgm_read_byte(&right[0]), 32, (int)pgm_read_byte(&right[0]), (int)pgm_read_byte(&right[1]));
      }
    } else {
        //perfect flow
        clearDisplay(0, 5, 0, 128);
        anaSteps = 5;
        Serial.println(F("Analyzer step 4 -> 5"));
        start = millis();
    }              
  }
}

/**
 * Finds the right digit
 */
long __p(int digit, long decvalcp, long power) {
  long tmp = (decvalcp - (decvalcp % power)) / power;

  if (!numDigitFound && tmp == 0) {
    digs[digit] = 11; //blank
  } else {
    numDigitFound = true;
    digs[digit] = tmp;
  }
  decvalcp = decvalcp - tmp * power;
  return decvalcp;
}

/**
 * Displays the Oxy percent value
 */
void displayValue(int decval) {  
  clearDisplay(0, 4, 0, 128);
  
  if (decval / 10000 == 1) {
    //prints digit 100.00 at
    digs[0] = 1;
    digs[1] = 0;
    digs[2] = 0;
    digs[3] = 10;
    digs[4] = 0;
    digs[5] = 0;
  } else {
    numDigitFound = false;
    decvalcp = decval;
    decvalcp = __p(0, decvalcp, 1000);
    decvalcp = __p(1, decvalcp, 100);
    if(digs[1]==11) {
      digs[1]=0;
      numDigitFound=true;
    }
    digs[2] = 10; // Point
    decvalcp = __p(3, decvalcp, 10);
    decvalcp = __p(4, decvalcp, 1);
  }
  
  digs[5] = 12;
  digs[6] = 11;
  digs[7] = 13;
  
  pos = 120;
  for (int i = 7; i >= 0; i--) {
    pos -= (int)pgm_read_byte(&digits[digs[i]][0]);
    OzOled.drawBitmap(&(digits[digs[i]][2]), pos, 0, (int)pgm_read_byte(&digits[digs[i]][0]), (int)pgm_read_byte(&digits[digs[i]][1]));
  }
}

/**
 * Update the status bar. progress variable value used.
 */
void progressBarUpdate() {
  if (progress != oldprogress) {
    Serial.print(F("Progress value : "));
    Serial.println(progress);

    //progres value changed. Do we need to update the display too?
    if (((int)(oldprogress * 0.28)) < ((int)(progress * 0.28))) {
      //repaint;
      OzOled.drawBitmap(&progressElement[2], 4 + ((int)(progress * 0.28)) * (int)pgm_read_byte(&progressElement[0]), 40, (int)pgm_read_byte(&progressElement[0]), (int)pgm_read_byte(&progressElement[1]));

      Serial.print(F("Repainting element @ (x,y): ("));
      Serial.print(4 + ((int)(progress * 0.28)) * (int)pgm_read_byte(&progressElement[0]));
      Serial.println(F(", 40)"));
    }    
    oldprogress = progress;
  }
}

/**
 * print the 
 */
void printTransitionMessage(const __FlashStringHelper* task, int currentStep, int nextStep){
  Serial.print(task);
  Serial.print(F(" step "));
  Serial.print(currentStep);
  Serial.print(F(" -> "));
  Serial.println(nextStep);  
}

/**
 * Reads value and display value each 280ms => ~5 times per second during about 10s
 */
void analyzeTaskReadValue(const __FlashStringHelper* task, int currentStep, int nextStep) {
  if(millis()-start > 280){
    start = millis();
    progress++;    

    progressBarUpdate();

    if(progress==100){
      Serial.println(F("Calibration completed"));      
      progress = 0;
      anaSteps = nextStep; //Fade progress bar
      printTransitionMessage(task, currentStep, nextStep);      
      start = millis();      
    }else{
      if(progress % 10 == 0) {        
        mappedValue = map(analogRead(O2_CELL_ANALOG), 0, 1023, 0, 10000);
                
        //Do stuff related with task completion ... example display the value ;)
        displayValue(mappedValue); 
        
        Serial.print(F("O2 mapped value :"));
        Serial.println(mappedValue);        
      }      
    }
  }
}

/**
 * Fade the progress bar
 */
void analyzeTaskFadeProgressBar(const __FlashStringHelper* task, int currentStep, int nextStep) {
  if (millis() - start > 100) {
    if (gencnt < 15) {
      Serial.println(F("Fadding"));
  
      clearDisplay(5, 6, 8, 8 + gencnt * 4);
      clearDisplay(5, 6, 120 - gencnt * 4, 120);
      gencnt++;
      start = millis();
    } else {
      //Before leave... clean state machine variable anaSteps and set main loop to idle
      gencnt = 0;
      anaSteps = nextStep;
      printTransitionMessage(task, currentStep, nextStep);
    }
  }  
}

/**
 * The analyze workflow 
 */
boolean analyzeTask() {
  switch(anaSteps) {
    case 0:
      analyzeTaskDoesFlowExists();
      break;
    case 1: 
      analyzeTaskDisplayMessageToEnableNitroxFlow();           
      break;      
    case 2: 
      analyzeTaskBlinkingText();
      break;    
    case 3:
      clearDisplay(0, 4, 0, 128);
      OzOled.printString("Flow indicator", 1, 0, 14); 
      OzOled.printString("more   |  less", 1, 1, 14); 
      start = millis();
      Serial.println(F("Analyzer step 3 -> 4"));
      anaSteps = 4;
    case 4:
      analyzeTaskFlowRegulation();      
      break;
    case 5:
      analyzeTaskReadValue(F("Analyzer"), 5, 6);
      break;
    case 6:
      analyzeTaskFadeProgressBar(F("Analyzer"),6, 7);
      break;      
    case 7:
      //Before leave... clean state machine variable anaSteps and set main loop to idle
      anaSteps = 0;
      Serial.println(F("Fadding finished"));
      return true;
  }
  return false;
}

/**
 * Calibration workflow
 */
boolean calibrationTask() {
  switch(anaSteps) {
    case 0:
      analyzeTaskReadValue(F("Calibration"), 0, 1);
      break;
    case 1:
      analyzeTaskFadeProgressBar(F("Calibration"), 1, 2);
      break;      
    case 2:
      //Before leave... clean state machine variable anaSteps and set main loop to idle
      anaSteps = 0;
      Serial.println(F("Fadding finished"));
      return true;
  }  
  return false;
}


/**
 * The main arduino loop
 */
void loop() {
  //Check start analyse button
  checkAndDebounce (0, LEFT_BUTTON_PIN, &actionOnStartButtonClick);
  
  //Check calibrate button
  checkAndDebounce (1, RIGHT_BUTTON_PIN, &actionOnCalibrationButtonClick);

  //Software main state loop
  switch (state) {
    case 0:
      displaySplashScreen();
      break;
    case 1:
      if(nonBlockingDelayElapsed(500)) {
        state = 2;
        Serial.println(F("Changing state : 1 -> 2"));
      }
      break;
    case 2:
      displayMainScreen();
      break;
    case 3: 
      if(analyzeTask()){
        Serial.println(F("Changing state : 3 -> Idle"));
        state = 1000;
      }
      break;
    case 4: //Start Calibration
      if(calibrationTask()){
        Serial.println(F("Changing state : 4 -> Idle"));   
        state = 1000;     
      }
      break;      
    default:
      break;
  }
}

