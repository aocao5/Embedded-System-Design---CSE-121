#ifndef DFROBOT_RGBLCD1602_H
#define DFROBOT_RGBLCD1602_H

#include "driver/i2c.h"

#define LCD_ADDRESS     0x3E   // Check if this is correct for your LCD
#define RGB_ADDRESS     0x62   // Check if this is correct for your RGB backlight
#define I2C_MASTER_TIMEOUT_MS 1000 // Set timeout (in milliseconds)

class DFRobot_RGBLCD1602 {
public:
    DFRobot_RGBLCD1602(i2c_port_t i2c_port);
    void init();
    void setCursor(uint8_t col, uint8_t row);
    void printstr(const char *str);
    void setRGB(uint8_t r, uint8_t g, uint8_t b);

private:
    i2c_port_t i2c_port;
    uint8_t lcd_address;
    uint8_t rgb_address;
    void sendCommand(uint8_t cmd);
    void sendData(uint8_t data);
};

#endif // DFROBOT_RGBLCD1602_H

