#ifndef DFROBOT_RGBLCD1602_H
#define DFROBOT_RGBLCD1602_H

#include "driver/i2c.h"

#define LCD_ADDR 0x3E // Replace with your specific LCD address if different
#define RGB_ADDR 0x62 // Replace with your specific RGB address if different

class DFRobot_RGBLCD1602 {
public:
    DFRobot_RGBLCD1602();
    ~DFRobot_RGBLCD1602();
    esp_err_t init();
    void setRGB(uint8_t r, uint8_t g, uint8_t b);
    void setCursor(uint8_t col, uint8_t row);
    void printstr(const char *str);

private:
    esp_err_t i2c_write(uint8_t address, uint8_t *data, size_t len);
    void command(uint8_t value);
    void writeChar(uint8_t value);
};

#endif // DFROBOT_RGBLCD1602_H

