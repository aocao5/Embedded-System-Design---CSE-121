// DFRobot_RGBLCD1602.cpp
#include "DFRobot_RGBLCD1602.h"
#include <string.h>

#define I2C_MASTER_TIMEOUT_MS 1000

DFRobot_RGBLCD1602::DFRobot_RGBLCD1602(i2c_port_t i2c_num, uint8_t lcd_addr, uint8_t rgb_addr)
    : _i2c_num(i2c_num), _lcd_addr(lcd_addr), _rgb_addr(rgb_addr) {}

void DFRobot_RGBLCD1602::init() {
    // Initialize I2C and the LCD
    i2c_config_t config = {};
    config.mode = I2C_MODE_MASTER;
    config.sda_io_num = 10;
    config.scl_io_num = 8;
    config.sda_pullup_en = GPIO_PULLUP_ENABLE;
    config.scl_pullup_en = GPIO_PULLUP_ENABLE;
    config.master.clk_speed = 100000;
    i2c_param_config(_i2c_num, &config);
    i2c_driver_install(_i2c_num, config.mode, 0, 0, 0);

    sendCommand(0x33); // Initialize LCD in 4-bit mode
    sendCommand(0x32); // Set to 4-bit mode
    sendCommand(0x28); // 2 line, 5x8 matrix
    sendCommand(0x0C); // Display on, cursor off
    sendCommand(0x06); // Increment cursor
    sendCommand(0x01); // Clear display
}


void DFRobot_RGBLCD1602::setRGB(uint8_t r, uint8_t g, uint8_t b) {
    // Set the RGB color of the LCD
    uint8_t rgb_command[] = {0x00, r, g, b};
    i2c_master_write_to_device(_i2c_num, _rgb_addr, rgb_command, sizeof(rgb_command), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

void DFRobot_RGBLCD1602::setCursor(uint8_t col, uint8_t row) {
    uint8_t offsets[] = {0x00, 0x40};
    sendCommand(0x80 | (col + offsets[row]));
}

void DFRobot_RGBLCD1602::printstr(const char* str) {
    while (*str) {
        sendData(*str++);
    }
}

void DFRobot_RGBLCD1602::sendCommand(uint8_t cmd) {
    uint8_t buffer[2] = {0x00, cmd};
    i2c_master_write_to_device(_i2c_num, _lcd_addr, buffer, sizeof(buffer), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

void DFRobot_RGBLCD1602::sendData(uint8_t data) {
    uint8_t buffer[2] = {0x40, data};
    i2c_master_write_to_device(_i2c_num, _lcd_addr, buffer, sizeof(buffer), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

