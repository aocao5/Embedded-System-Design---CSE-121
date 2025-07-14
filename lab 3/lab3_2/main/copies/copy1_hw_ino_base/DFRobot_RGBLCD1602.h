#ifndef DFROBOT_RGBLCD1602_H
#define DFROBOT_RGBLCD1602_H

#include "driver/i2c.h"
#include "esp_log.h"

// Default I2C address for the LCD
#define LCD_ADDRESS 0x3E
#define RGB_ADDRESS 0x62

class DFRobot_RGBLCD1602 {
public:
    DFRobot_RGBLCD1602(uint8_t lcd_cols, uint8_t lcd_rows);
    esp_err_t init();
    esp_err_t setRGB(uint8_t r, uint8_t g, uint8_t b);
    esp_err_t setCursor(uint8_t col, uint8_t row);
    esp_err_t printstr(const char *str);

private:
    esp_err_t writeCommand(uint8_t command);
    esp_err_t writeData(uint8_t data);
    esp_err_t i2cWrite(uint8_t address, uint8_t *data, size_t len);

    uint8_t _cols;
    uint8_t _rows;
};

#endif // DFROBOT_RGBLCD1602_H

