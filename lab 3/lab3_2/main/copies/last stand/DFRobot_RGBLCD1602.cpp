#include "DFRobot_RGBLCD1602.h"
#include "driver/i2c.h"  // ESP32 I2C library

#define I2C_PORT I2C_NUM_0
#define I2C_MASTER_TIMEOUT_MS 1000 
#define GPIO_NUM_10 10
#define GPIO_NUM_8 8

// Initialize the I2C for ESP32
void DFRobot_RGBLCD1602::init(){
    // Set up I2C configuration for ESP32
	
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = SDA_PIN;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = SCL_PIN;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 100000;
	conf.clk_flags = 0;
	
	i2c_param_config(I2C_PORT, &conf);
    i2c_driver_install(I2C_PORT, conf.mode, 0, 0, 0);
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

void DFRobot_RGBLCD1602::setRGB(uint8_t r, uint8_t g, uint8_t b) {
    setReg(REG_RED, r);
    setReg(REG_GREEN, g);
    setReg(REG_BLUE, b);
}

void DFRobot_RGBLCD1602::sendCommand(uint8_t cmd) {
    uint8_t buffer[2] = {0x00, cmd};
    i2c_master_write_to_device(i2c_port, lcd_address, buffer, sizeof(buffer), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

void DFRobot_RGBLCD1602::sendData(uint8_t data) {
    uint8_t buffer[2] = {0x40, data};
    i2c_master_write_to_device(i2c_port, lcd_address, buffer, sizeof(buffer), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

void DFRobot_RGBLCD1602::setReg(uint8_t reg, uint8_t value) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (LCD_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_write_byte(cmd, value, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_PORT, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
}


DFRobot_RGBLCD1602::DFRobot_RGBLCD1602(i2c_port_t i2c_port, uint8_t lcd_address, uint8_t rgb_address)
    : i2c_port(i2c_port), lcd_address(lcd_address), rgb_address(rgb_address) {}

