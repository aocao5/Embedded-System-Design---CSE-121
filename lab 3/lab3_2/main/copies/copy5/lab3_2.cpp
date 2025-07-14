#include "DFRobot_RGBLCD1602.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" void app_main() {
    DFRobot_RGBLCD1602 lcd(I2C_NUM_0);
    lcd.init();
    lcd.setRGB(255, 0, 0); // Red color for the backlight
    lcd.setCursor(0, 0);    // First row, first column
    lcd.printstr("Hello, CSE121!!");

    lcd.setCursor(0, 1);    // Second row, first column
    lcd.printstr("Cao");

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
