// DFRobot_RGBLCD1602.h
#ifndef DFROBOT_RGBLCD1602_H
#define DFROBOT_RGBLCD1602_H

#include "driver/i2c.h"

class DFRobot_RGBLCD1602 {
public:
    DFRobot_RGBLCD1602(i2c_port_t i2c_num, uint8_t lcd_addr, uint8_t rgb_addr);
    void init();
    void setRGB(const char* text);
    void setCursor(uint8_t col, uint8_t row);
    void printstr(const char* str);

private:
    void sendCommand(uint8_t cmd);
    void sendData(uint8_t data);
    i2c_port_t _i2c_num;
    uint8_t _lcd_addr;
    uint8_t _rgb_addr;
};

#endif // DFROBOT_RGBLCD1602_H

