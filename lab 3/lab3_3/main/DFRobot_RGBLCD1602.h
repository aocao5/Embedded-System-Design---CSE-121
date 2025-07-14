/*!
 * @file DFRobot_RGBLCD1602.h
 * @brief DFRobot RGB LCD library for ESP32-C3
 */

#ifndef __DFROBOT_RGBLCD1602_H__
#define __DFROBOT_RGBLCD1602_H__

#include <stdint.h>
#include <stddef.h>

#define LCD_ADDRESS     (0x7c >> 1)  // Default LCD I2C address
#define RGB_ADDRESS     (0x2D)  // Default RGB I2C address

#define REG_RED         0x04
#define REG_GREEN       0x03
#define REG_BLUE        0x02

#define REG_MODE1       0x00
#define REG_MODE2       0x01
#define REG_OUTPUT      0x08

// LCD Commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME   0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT  0x10
#define LCD_FUNCTIONSET  0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

#define LCD_DISPLAYON    0x04
#define LCD_ENTRYLEFT    0x02
#define LCD_2LINE        0x08

class DFRobot_RGBLCD1602 {
public:
    DFRobot_RGBLCD1602(uint8_t cols, uint8_t rows, uint8_t lcdAddr, uint8_t rgbAddr);
    void init();
    void clear();
    void setCursor(uint8_t col, uint8_t row);
    void print(const char *str);
    void setRGB(uint8_t r, uint8_t g, uint8_t b);
    void setBrightness(uint8_t brightness); // Add declaration here

private:
    void sendCommand(uint8_t value);
    void sendData(uint8_t value);
    void writeRegister(uint8_t reg, uint8_t value);
    void i2cSend(uint8_t address, uint8_t *data, size_t len);
    void setupLCD();
    void setupRGB(uint8_t r, uint8_t g, uint8_t b);

    uint8_t _lcdAddr;
    uint8_t _rgbAddr;
    uint8_t _cols;
    uint8_t _rows;

    // Add member variables for brightness control
    uint8_t _currentRed;
    uint8_t _currentGreen;
    uint8_t _currentBlue;
};



#endif

