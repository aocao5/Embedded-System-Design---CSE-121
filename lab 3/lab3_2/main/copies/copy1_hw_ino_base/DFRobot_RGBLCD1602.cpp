#include "DFRobot_RGBLCD1602.h"

#define I2C_MASTER_NUM I2C_NUM_0  // I2C port number for ESP32
#define I2C_MASTER_TIMEOUT_MS 1000

DFRobot_RGBLCD1602::DFRobot_RGBLCD1602(uint8_t lcd_cols, uint8_t lcd_rows) 
    : _cols(lcd_cols), _rows(lcd_rows) {}

esp_err_t DFRobot_RGBLCD1602::init() {
    esp_err_t ret;

    // Initialization commands
    ret = writeCommand(0x01); // Clear display
    if (ret != ESP_OK) return ret;

    ret = writeCommand(0x02); // Return home
    if (ret != ESP_OK) return ret;

    ret = writeCommand(0x0C); // Display ON, cursor OFF
    if (ret != ESP_OK) return ret;

    return ESP_OK;
}

esp_err_t DFRobot_RGBLCD1602::setRGB(uint8_t r, uint8_t g, uint8_t b) {
    uint8_t data[2];
    data[0] = 0x00;   // Mode register address for setting RGB color
    data[1] = 0x00;   // RGB setting data (dummy example)

    // Send RGB settings for each color channel
    i2cWrite(RGB_ADDRESS, data, sizeof(data));
    data[0] = 0x04; data[1] = r;
    i2cWrite(RGB_ADDRESS, data, sizeof(data));
    data[0] = 0x03; data[1] = g;
    i2cWrite(RGB_ADDRESS, data, sizeof(data));
    data[0] = 0x02; data[1] = b;
    return i2cWrite(RGB_ADDRESS, data, sizeof(data));
}

esp_err_t DFRobot_RGBLCD1602::setCursor(uint8_t col, uint8_t row) {
    uint8_t row_offsets[] = {0x00, 0x40};
    return writeCommand(0x80 | (col + row_offsets[row]));
}

esp_err_t DFRobot_RGBLCD1602::printstr(const char *str) {
    while (*str) {
        writeData(*str++);
    }
    return ESP_OK;
}

esp_err_t DFRobot_RGBLCD1602::writeCommand(uint8_t command) {
    uint8_t data[2] = {0x80, command};
    return i2cWrite(LCD_ADDRESS, data, 2);
}

esp_err_t DFRobot_RGBLCD1602::writeData(uint8_t dataByte) {
    uint8_t data[2] = {0x40, dataByte};
    return i2cWrite(LCD_ADDRESS, data, 2);
}

esp_err_t DFRobot_RGBLCD1602::i2cWrite(uint8_t address, uint8_t *data, size_t len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write(cmd, data, len, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));
    i2c_cmd_link_delete(cmd);
    return ret;
}

