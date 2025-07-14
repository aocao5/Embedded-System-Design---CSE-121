/*!
 * @file DFRobot_RGBLCD1602.cpp
 * @brief DFRobot RGB LCD adapted for ESP32-C3
 */

#include "DFRobot_RGBLCD1602.h"
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Define your I2C pins and settings
#define I2C_SCL_PIN 8            // ESP32 SCL pin
#define I2C_SDA_PIN 10           // ESP32 SDA pin
#define I2C_PORT I2C_NUM_0       // I2C port number
#define I2C_FREQ_HZ 100000       // Frequency (100 kHz)

// RGB color definitions
const uint8_t RGB_COLORS[4][3] = {
    {255, 255, 255},    // white
    {255, 0, 0},        // red
    {0, 255, 0},        // green
    {0, 0, 255}         // blue
};

DFRobot_RGBLCD1602::DFRobot_RGBLCD1602(uint8_t cols, uint8_t rows, uint8_t lcdAddr, uint8_t rgbAddr)
    : _lcdAddr(lcdAddr), _rgbAddr(rgbAddr), _cols(cols), _rows(rows) {}

void DFRobot_RGBLCD1602::init() {
    vTaskDelay(200 / portTICK_PERIOD_MS);  // Delay to allow LCD power-up

    // Configure I2C parameters
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;                       // Set I2C to master mode
    conf.sda_io_num = static_cast<gpio_num_t>(I2C_SDA_PIN);  // SDA pin
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;           // Enable pull-up on SDA
    conf.scl_io_num = static_cast<gpio_num_t>(I2C_SCL_PIN);  // SCL pin
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;           // Enable pull-up on SCL
    conf.clk_flags = 0;                                // Default clock flags
    conf.master.clk_speed = I2C_FREQ_HZ;               // Set clock speed for I2C master

    ESP_ERROR_CHECK(i2c_param_config(I2C_PORT, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_PORT, conf.mode, 0, 0, 0));

    // Initialize LCD and RGB
    setupLCD();
    setupRGB(255, 255, 255);  // Default to white
}

void DFRobot_RGBLCD1602::setupLCD() {
    sendCommand(0x30);  // Wake up
    vTaskDelay(5 / portTICK_PERIOD_MS);
    sendCommand(0x30);  // Repeat wake-up sequence
    vTaskDelay(5 / portTICK_PERIOD_MS);
    sendCommand(0x30);
    vTaskDelay(5 / portTICK_PERIOD_MS);
    sendCommand(0x20);  // Set to 4-bit mode

    // Configure LCD settings
    sendCommand(LCD_FUNCTIONSET | LCD_2LINE);
    sendCommand(LCD_DISPLAYCONTROL | LCD_DISPLAYON);
    clear();
    sendCommand(LCD_ENTRYMODESET | LCD_ENTRYLEFT);
}



void DFRobot_RGBLCD1602::setupRGB(uint8_t r, uint8_t g, uint8_t b) {
    writeRegister(REG_MODE1, 0x00);
    writeRegister(REG_OUTPUT, 0xFF);
    writeRegister(REG_MODE2, 0x20);
    setRGB(r, g, b);
}

/*
void DFRobot_RGBLCD1602::setRGB(uint8_t r, uint8_t g, uint8_t b) {
    _currentRed = r;
    _currentGreen = g;
    _currentBlue = b;

    writeRegister(REG_RED, r);
    writeRegister(REG_GREEN, g);
    writeRegister(REG_BLUE, b);
}

void DFRobot_RGBLCD1602::setBrightness(uint8_t brightness) {

    // Scale the stored RGB values
    uint8_t scaledRed = (_currentRed * brightness) / 255;
    uint8_t scaledGreen = (_currentGreen * brightness) / 255;
    uint8_t scaledBlue = (_currentBlue * brightness) / 255;

    // Apply the scaled brightness
    setRGB(scaledRed, scaledGreen, scaledBlue);
}
*/

void DFRobot_RGBLCD1602::setRGB(uint8_t r, uint8_t g, uint8_t b) {
    // Ensure the RGB values are within the valid range (0–255)
    if (r > 255) r = 255;
    if (g > 255) g = 255;
    if (b > 255) b = 255;

    // Save the current RGB values for brightness scaling
    _currentRed = r;
    _currentGreen = g;
    _currentBlue = b;

    // Write the RGB values to the backlight controller
    writeRegister(REG_RED, r);
    writeRegister(REG_GREEN, g);
    writeRegister(REG_BLUE, b);

    // Ensure the backlight is fully enabled
    writeRegister(REG_MODE1, 0x00);  // Normal operation
    writeRegister(REG_OUTPUT, 0xFF); // Enable all LED drivers
    writeRegister(REG_MODE2, 0x20);  // Set MODE2 for RGB control
}

void DFRobot_RGBLCD1602::setBrightness(uint8_t brightness) {
    if (brightness > 255) brightness = 255;  // Clamp brightness to max value

    // Scale the current RGB values
    uint8_t scaledRed = (_currentRed * brightness) / 255;
    uint8_t scaledGreen = (_currentGreen * brightness) / 255;
    uint8_t scaledBlue = (_currentBlue * brightness) / 255;

    // Apply the scaled brightness to the backlight
    setRGB(scaledRed, scaledGreen, scaledBlue);
}


void DFRobot_RGBLCD1602::setCursor(uint8_t col, uint8_t row) {
    uint8_t offsets[] = {0x00, 0x40};
    sendCommand(LCD_SETDDRAMADDR | (col + offsets[row]));
}

void DFRobot_RGBLCD1602::print(const char *str) {
    while (*str) {
        sendData(*str++);
    }
}

void DFRobot_RGBLCD1602::clear() {
    sendCommand(LCD_CLEARDISPLAY);
    vTaskDelay(2 / portTICK_PERIOD_MS);  // Wait for the command to complete
}

void DFRobot_RGBLCD1602::sendCommand(uint8_t value) {
    uint8_t buffer[2] = {0x80, value};
    i2cSend(_lcdAddr, buffer, 2);
}

void DFRobot_RGBLCD1602::sendData(uint8_t value) {
    uint8_t buffer[2] = {0x40, value};
    i2cSend(_lcdAddr, buffer, 2);
}

void DFRobot_RGBLCD1602::writeRegister(uint8_t reg, uint8_t value) {
    uint8_t buffer[2] = {reg, value};
    i2cSend(_rgbAddr, buffer, 2);
}

void DFRobot_RGBLCD1602::i2cSend(uint8_t address, uint8_t *data, size_t len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write(cmd, data, len, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_PORT, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
}

