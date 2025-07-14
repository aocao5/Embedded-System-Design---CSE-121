#include "DFRobot_RGBLCD1602.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <cstring>

DFRobot_RGBLCD1602::DFRobot_RGBLCD1602(i2c_port_t i2c_port)
    : i2c_port(i2c_port), lcd_address(LCD_ADDRESS), rgb_address(RGB_ADDRESS) {}

void DFRobot_RGBLCD1602::init() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = GPIO_NUM_10,
        .scl_io_num = GPIO_NUM_8,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000  // 100kHz
    };
    i2c_param_config(i2c_port, &conf);
    i2c_driver_install(i2c_port, conf.mode, 0, 0, 0);

    sendCommand(0x01); // Clear display
    vTaskDelay(pdMS_TO_TICKS(2));
    sendCommand(0x0C); // Display ON, cursor OFF
    sendCommand(0x06); // Entry mode set
}

void DFRobot_RGBLCD1602::setCursor(uint8_t col, uint8_t row) {
    static const uint8_t row_offsets[] = { 0x00, 0x40 };
    sendCommand(0x80 | (col + row_offsets[row]));
}

void DFRobot_RGBLCD1602::printstr(const char *str) {
    while (*str) {
        sendData(*str++);
    }
}

void DFRobot_RGBLCD1602::setRGB(uint8_t r, uint8_t g, uint8_t b) {
    uint8_t buffer[2];

    buffer[0] = 0x00;
    buffer[1] = 0x00;  // Command to set RGB mode
    i2c_master_write_to_device(i2c_port, rgb_address, buffer, 2, pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));

    buffer[0] = 0x04; buffer[1] = r;
    i2c_master_write_to_device(i2c_port, rgb_address, buffer, 2, pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));
    buffer[0] = 0x03; buffer[1] = g;
    i2c_master_write_to_device(i2c_port, rgb_address, buffer, 2, pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));
    buffer[0] = 0x02; buffer[1] = b;
    i2c_master_write_to_device(i2c_port, rgb_address, buffer, 2, pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));
}

void DFRobot_RGBLCD1602::sendCommand(uint8_t cmd) {
    i2c_cmd_handle_t handle = i2c_cmd_link_create();
    i2c_master_start(handle);
    i2c_master_write_byte(handle, (lcd_address << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(handle, 0x80, true);  // Control byte for command
    i2c_master_write_byte(handle, cmd, true);
    i2c_master_stop(handle);
    i2c_master_cmd_begin(i2c_port, handle, pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));
    i2c_cmd_link_delete(handle);
}

void DFRobot_RGBLCD1602::sendData(uint8_t data) {
    i2c_cmd_handle_t handle = i2c_cmd_link_create();
    i2c_master_start(handle);
    i2c_master_write_byte(handle, (lcd_address << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(handle, 0x40, true);  // Control byte for data
    i2c_master_write_byte(handle, data, true);
    i2c_master_stop(handle);
    i2c_master_cmd_begin(i2c_port, handle, pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));
    i2c_cmd_link_delete(handle);
}

