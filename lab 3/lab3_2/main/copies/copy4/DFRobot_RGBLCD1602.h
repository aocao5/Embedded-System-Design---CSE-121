#ifndef __DFRobot_RGBLCD1602_H__
#define __DFRobot_RGBLCD1602_H__

#include <stdio.h>
#include <string.h>
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// LCD Commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// Flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// Flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// Flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// Flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

class DFRobot_RGBLCD1602 {
public:
    DFRobot_RGBLCD1602(i2c_port_t i2c_port, uint8_t lcd_addr, uint8_t rgb_addr);
    
    void init();
    void clear();
    void home();
    void noDisplay();
    void display();
    void stopBlink();
    void blink();
    void noCursor();
    void cursor();
    void scrollDisplayLeft();
    void scrollDisplayRight();
    void leftToRight();
    void rightToLeft();
    void noAutoscroll();
    void autoscroll();
    void setCursor(uint8_t col, uint8_t row);
    void setRGB(uint8_t r, uint8_t g, uint8_t b);
    void setColor(uint8_t color);
    void setBacklight(bool mode);
    void printstr(const char* str);
    
private:
    void begin(uint8_t rows, uint8_t charsize = LCD_5x8DOTS);
    void command(uint8_t value);
    void send(uint8_t value, uint8_t mode);
    void write(uint8_t value);
    void setReg(uint8_t addr, uint8_t data);
    esp_err_t i2c_write_bytes(uint8_t addr, uint8_t* data, size_t len);
    
    uint8_t _showFunction;
    uint8_t _showControl;
    uint8_t _showMode;
    uint8_t _numLines;
    uint8_t _currLine;
    uint8_t _lcdAddr;
    uint8_t _rgbAddr;
    i2c_port_t _i2c_port;
};

#endif
