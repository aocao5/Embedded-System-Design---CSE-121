#include "DFRobot_RGBLCD1602.h"
#include "DFRobot_RGBLCD1602.cpp"
#include <stdio.h>
#include <cmath>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"

// Constants for I2C and sensor
#define I2C_MASTER_SCL_IO 8
#define I2C_MASTER_SDA_IO 10
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 100000
#define SHTC3_ADDR 0x70
#define SHTC3_WAKEUP_CMD 0x3517
#define SHTC3_MEAS_CMD 0x7CA2
#define SHTC3_SLEEP_CMD 0x8098

// Logger tag
#define TAG "SHTC3_SENSOR"

// Helper functions
uint8_t calculate_crc(uint8_t data[], int len) {
    const uint8_t polynomial = 0x31;
    uint8_t crc = 0xFF;
    for (int j = 0; j < len; j++) {
        crc ^= data[j];
        for (int i = 0; i < 8; i++) {
            crc = (crc & 0x80) ? (crc << 1) ^ polynomial : (crc << 1);
        }
    }
    return crc;
}

esp_err_t i2c_master_init() {

    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    
    i2c_param_config(I2C_MASTER_NUM, &conf);
    return i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

esp_err_t shtc3_send_command(uint16_t cmd) {
    uint8_t data[2] = {(uint8_t)(cmd >> 8), (uint8_t)cmd};
    return i2c_master_write_to_device(I2C_MASTER_NUM, SHTC3_ADDR, data, sizeof(data), 1000 / portTICK_PERIOD_MS);
}

float shtc3_read_temperature() {
    uint8_t data[6];
    shtc3_send_command(SHTC3_WAKEUP_CMD);
    shtc3_send_command(SHTC3_MEAS_CMD);
    vTaskDelay(pdMS_TO_TICKS(20));

    if (i2c_master_read_from_device(I2C_MASTER_NUM, SHTC3_ADDR, data, 6, 1000 / portTICK_PERIOD_MS) == ESP_OK) {
        uint16_t temp_raw = (data[0] << 8) | data[1];
        if (calculate_crc(data, 2) == data[2]) {
            return -45.0 + 175.0 * (float(temp_raw) / 65535.0);
        } else {
            ESP_LOGE(TAG, "Temperature CRC error");
        }
    }
    return -1.0;
}

float shtc3_read_humidity() {
    uint8_t data[6];
    shtc3_send_command(SHTC3_WAKEUP_CMD);
    shtc3_send_command(SHTC3_MEAS_CMD);
    vTaskDelay(pdMS_TO_TICKS(20));

    if (i2c_master_read_from_device(I2C_MASTER_NUM, SHTC3_ADDR, data, 6, 1000 / portTICK_PERIOD_MS) == ESP_OK) {
        uint16_t humidity_raw = (data[3] << 8) | data[4];
        if (calculate_crc(&data[3], 2) == data[5]) {
            return 100.0 * (float(humidity_raw) / 65535.0);
        } else {
            ESP_LOGE(TAG, "Humidity CRC error");
        }
    }
    return -1.0;
}

// Main application
extern "C" void app_main() {
    ESP_ERROR_CHECK(i2c_master_init());

    DFRobot_RGBLCD1602 lcd(16, 2, LCD_ADDRESS, RGB_ADDRESS);
    lcd.init();
    lcd.setRGB(0, 255, 0);

    while (true) {
        float temperature = shtc3_read_temperature();
        float humidity = shtc3_read_humidity();

        lcd.clear();
        lcd.setCursor(0, 0);

        if (temperature != -1.0) {
            char temp_str[16];
            snprintf(temp_str, sizeof(temp_str), "  Temp: %.1f C", temperature);
            lcd.print(temp_str);
        } else {
            lcd.print("Temp: Error");
        }

        lcd.setCursor(0, 1);

        if (humidity != -1.0) {
            char hum_str[16];
            snprintf(hum_str, sizeof(hum_str), "Humidity: %.1f%%", humidity);
            lcd.print(hum_str);
        } else {
            lcd.print("Humidity: Err");
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
