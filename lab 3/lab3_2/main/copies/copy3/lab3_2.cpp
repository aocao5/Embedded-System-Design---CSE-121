#include "DFRobot_RGBLCD1602.h"
#include "DFRobot_RGBLCD1602.cpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" void app_main() {
    DFRobot_RGBLCD1602 lcd(I2C_NUM_0, 0x27, 0x62);
	
	lcd.init();
	lcd.setRGB(255, 0, 0);	
    
    // Initialize the LCD
	while (true) {	
		//lcd.init();
		lcd.setCursor(0, 0);
		lcd.printstr("Hello, CSE121!!");
        
		lcd.setCursor(0, 1);     // Set cursor to start of the first row
        lcd.printstr("Cao");  // Replace with your actual last name

        //vTaskDelay(pdMS_TO_TICKS(1000)); // Keep the program running
    }
}

