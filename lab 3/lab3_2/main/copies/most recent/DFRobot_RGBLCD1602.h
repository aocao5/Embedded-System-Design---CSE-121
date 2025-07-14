#ifndef DFROBOT_RGBLCD1602_H
#define DFROBOT_RGBLCD1602_H

#include <stdint.h>
#include "driver/i2c.h"

#define LCD_ADDRESS 0x3E // LCD address
#define RGB_ADDRESS 0x62 // RGB controller address

#define REG_RED 0x04     // Register for red color
#define REG_GREEN 0x03   // Register for green color
#define REG_BLUE 0x02    // Register for blue color

#define I2C_PORT I2C_NUM_0 // Define which I2C port to use
#define SDA_PIN 8         // Define SDA pin for ESP32
#define SCL_PIN 9         // Define SCL pin for ESP32
#define I2C_MASTER_TIMEOUT_MS 1000

#define LCD_FUNCTIONSET 0x20  // Example value, replace with the correct one
#define LCD_DISPLAYCONTROL 0x08  // Example value
#define LCD_CLEARDISPLAY 0x01  // Command to clear the display

// Add these as private member variables in your class
uint8_t _showfunction;
uint8_t _showcontrol;
uint8_t _showmode;

class DFRobot_RGBLCD1602 {
public:
	DFRobot_RGBLCD1602(i2c_port_t i2c_port, uint8_t lcd_address, uint8_t rgb_address);
    void init();
    void setRGB(uint8_t r, uint8_t g, uint8_t b);
    void setCursor(uint8_t col, uint8_t row);
    void printstr(const char *str);
	esp_err_t sendCommand(uint8_t cmd);
	void sendData(uint8_t data);
private:
	//void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);
    void setReg(uint8_t reg, uint8_t value);

    i2c_port_t i2c_port;
    uint8_t lcd_address;
    uint8_t rgb_address;
	void display();
	void clear();
};
#endif

