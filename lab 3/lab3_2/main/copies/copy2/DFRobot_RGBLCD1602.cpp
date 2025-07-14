#include "DFRobot_RGBLCD1602.h"
#include "driver/i2c.h"

#define I2C_MASTER_SCL_IO           8    // GPIO for SCL
#define I2C_MASTER_SDA_IO           10   // GPIO for SDA
#define I2C_MASTER_NUM              I2C_NUM_0
#define I2C_MASTER_FREQ_HZ          100000

DFRobot_RGBLCD1602::DFRobot_RGBLCD1602() {
    // Constructor implementation, if needed
}

DFRobot_RGBLCD1602::~DFRobot_RGBLCD1602() {
    // Destructor implementation, if needed
}

esp_err_t DFRobot_RGBLCD1602::init() {
    
	i2c_config_t conf = {};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = GPIO_NUM_10; //GPIO_NUM_21;  // Replace with your SDA pin
    conf.scl_io_num = GPIO_NUM_8; //GPIO_NUM_21;  // Replace with your SCL pin
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 100000; // Typical I2C clock speed

    // Configure and install I2C driver
    esp_err_t ret = i2c_param_config(I2C_MASTER_NUM, &conf);
    //if (ret != ESP_OK) return ret;

    ret = i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
    return ret;

	/*i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    conf.clk_flags = 0;
	
	i2c_param_config(I2C_NUM_0, &conf);
	i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0);
    
	ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0));

    // Send initialization commands to LCD
    command(0x80); // Example command; replace with actual init sequence for LCD
    return ESP_OK; */
}

void DFRobot_RGBLCD1602::setRGB(uint8_t r, uint8_t g, uint8_t b) {
    uint8_t data[2];
    data[0] = 0x00; // command
    data[1] = r;
    i2c_write(RGB_ADDR, data, 2);
    data[1] = g;
    i2c_write(RGB_ADDR, data, 2);
    data[1] = b;
    i2c_write(RGB_ADDR, data, 2);
}

void DFRobot_RGBLCD1602::setCursor(uint8_t col, uint8_t row) {
    uint8_t pos = (row == 0) ? col | 0x80 : col | 0xC0;
    command(pos);
}

void DFRobot_RGBLCD1602::printstr(const char *str) {
    while (*str) {
        writeChar(*str++);
    }
}

esp_err_t DFRobot_RGBLCD1602::i2c_write(uint8_t address, uint8_t *data, size_t len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write(cmd, data, len, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

void DFRobot_RGBLCD1602::command(uint8_t value) {
    uint8_t data[2] = {0x80, value};
    i2c_write(LCD_ADDR, data, 2);
}

void DFRobot_RGBLCD1602::writeChar(uint8_t value) {
    uint8_t data[2] = {0x40, value};
    i2c_write(LCD_ADDR, data, 2);
}

