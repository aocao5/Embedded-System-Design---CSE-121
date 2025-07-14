#include "DFRobot_RGBLCD1602.h"
#include "driver/i2c.h"  // ESP32 I2C library
#include "esp_log.h"
#include "esp_err.h"

#define I2C_PORT I2C_NUM_0
#define I2C_MASTER_TIMEOUT_MS 1000 
#define GPIO_SCL 8
#define GPIO_SDA 10

// Initialize the I2C for ESP32
void DFRobot_RGBLCD1602::init(){
    // Set up I2C configuration for ESP32
	
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = GPIO_SDA;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = GPIO_SCL;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 50000;
	conf.clk_flags = 0;
	
	i2c_param_config(I2C_PORT, &conf);
    i2c_driver_install(I2C_PORT, conf.mode, 0, 0, 0);

    vTaskDelay(50 / portTICK_PERIOD_MS); // Wait for the display to power up

    // Revised initialization sequence
    sendCommand(0x30); // Wake up
    vTaskDelay(5 / portTICK_PERIOD_MS); // Delay
    sendCommand(0x30); // Wake up again
    vTaskDelay(5 / portTICK_PERIOD_MS); // Delay
    sendCommand(0x30); // Wake up a third time
    vTaskDelay(5 / portTICK_PERIOD_MS); // Delay

    sendCommand(0x20); // Set to 4-bit mode
    sendCommand(LCD_FUNCTIONSET | 0x08); // Function set: 2-line, 5x8 dots
    sendCommand(LCD_DISPLAYCONTROL | 0x04); // Display ON, cursor OFF, blink OFF
    sendCommand(LCD_CLEARDISPLAY); // Clear the display
    vTaskDelay(2 / portTICK_PERIOD_MS); // Delay to let the display clear
    sendCommand(0x06); // Entry mode set: increment cursor

	setRGB(255, 255, 255);

/*
	setRGB(255, 255, 255);
	//sendCommand(LCD_FUNCTIONSET | _showfunction);

   	sendCommand(0x0C); // Power on, display on, cursor off

    // Wait for the display to power up
    vTaskDelay(50 / portTICK_PERIOD_MS);

    // Initialize the display settings
    sendCommand(0x33); // Initialize to 4-bit mode
    sendCommand(0x32); // Set to 4-bit mode again
    sendCommand(LCD_FUNCTIONSET | 0x08); // 2-line, 5x8 dot matrix
    sendCommand(LCD_DISPLAYCONTROL | 0x04); // Display ON, cursor OFF, blink OFF
    sendCommand(LCD_CLEARDISPLAY); // Clear the display
    vTaskDelay(2 / portTICK_PERIOD_MS); // Delay to let the display clear
    sendCommand(0x06); // Set entry mode, increment cursor
*/	
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

/*
void DFRobot_RGBLCD1602::sendCommand(uint8_t cmd) {
    uint8_t buffer[2] = {0x00, cmd};
    i2c_master_write_to_device(i2c_port, lcd_address, buffer, sizeof(buffer), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}
*/

esp_err_t DFRobot_RGBLCD1602::sendCommand(uint8_t value) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (LCD_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, value, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_PORT, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    if (ret == ESP_OK) {
        ESP_LOGI("I2C", "Command 0x%02X sent successfully", value);
    } else {
        ESP_LOGE("I2C", "Error sending command 0x%02X: %s", value, esp_err_to_name(ret));
    }
    return ret;
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

void DFRobot_RGBLCD1602::display() {
    _showcontrol |= LCD_DISPLAYCONTROL;
    sendCommand(LCD_DISPLAYCONTROL | _showcontrol);
}

void DFRobot_RGBLCD1602::clear() {
    sendCommand(LCD_CLEARDISPLAY);  // Clear display command
    vTaskDelay(2 / portTICK_PERIOD_MS);  // Delay to allow the display to clear
}

