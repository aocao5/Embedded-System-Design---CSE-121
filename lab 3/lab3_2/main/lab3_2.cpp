#include "DFRobot_RGBLCD1602.h"
#include "DFRobot_RGBLCD1602.cpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" void app_main() {
	DFRobot_RGBLCD1602 lcd(16, 2, LCD_ADDRESS, RGB_ADDRESS);
	
	lcd.setRGB(230, 255, 255);	
	lcd.setBrightness(200);	

    
    // Initialize the LCD
	while (true) {	
		lcd.init();
		lcd.print("Hello, CSE121!!!");
	        
		lcd.setCursor(0, 1);     // Set cursor to start of the first row
        lcd.print("cao");  // Replace with your actual last name

        vTaskDelay(5000 / portTICK_PERIOD_MS); // Keep the program running
    }
}

