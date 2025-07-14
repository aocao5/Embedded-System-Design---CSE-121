#include <stdio.h>
#include "driver/i2c.h"
#include "DFRobot_RGBLCD1602.h"

// I2C configuration
#define I2C_MASTER_SCL_IO 8         // GPIO number for I2C SCL
#define I2C_MASTER_SDA_IO 10        // GPIO number for I2C SDA
#define I2C_MASTER_NUM I2C_NUM_0    // I2C port number for master
#define I2C_MASTER_FREQ_HZ 100000   // I2C master clock frequency
#define I2C_MASTER_TX_BUF_DISABLE 0 // I2C master does not need buffer
#define I2C_MASTER_RX_BUF_DISABLE 0 // I2C master does not need buffer

DFRobot_RGBLCD1602 lcd(16, 2);      // Initialize display with 16 columns and 2 rows

extern "C" void app_main() {
    // I2C configuration setup
    /*
	i2c_config_t conf = {};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
	*/

	i2c_config_t conf = {};
	conf.mode = I2C_MODE_MASTER;
	conf.sda_io_num = 10;             // SDA pin
	conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
	conf.scl_io_num = 8;              // SCL pin
	conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
	conf.master.clk_speed = 100000;   // Set I2C clock frequency
	i2c_param_config(I2C_NUM_0, &conf);
	i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0);


    lcd.init();          // Initialize the LCD
    lcd.setRGB(0, 128, 64); // Set color

    // Display "Hello CSE121!" and your last name on the LCD
    lcd.setCursor(0, 0);
    lcd.printstr("Hello CSE121!");
    lcd.setCursor(0, 1);
    lcd.printstr("Cao");   // Replace with your last name

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000)); // Wait 1 second
    }
}


