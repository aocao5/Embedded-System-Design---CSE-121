#include "DFRobot_RGBLCD1602.h"
#include "driver/i2c.h"

extern "C" void app_main() {
    // Configure I2C
   
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = 8;  // Adjust these pins according to your setup
    conf.scl_io_num = 10;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 100000;
    conf.clk_flags = 0;
 
    i2c_param_config(I2C_NUM_0, &conf);
    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
    
    DFRobot_RGBLCD1602 lcd(I2C_NUM_0, 0x27, 0x62);
    
    lcd.init();
    lcd.setRGB(255, 0, 0);
    
    while (true) {
        lcd.setCursor(0, 0);
        lcd.printstr("Hello, CSE121!!");
        lcd.setCursor(0, 1);
        lcd.printstr("Cao");
    }
}
