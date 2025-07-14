#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <esp_timer.h>

// Pin definitions
#define TRIG_PIN GPIO_NUM_4
#define ECHO_PIN GPIO_NUM_1

// Speed of sound at 0°C is 331.3 m/s, it increases by 0.6 m/s per °C
float get_speed_of_sound(float temperature) {
    return 331.3 + 0.6 * temperature;
}

float measure_distance(float temperature) {
    for (int retries = 0; retries < 3; retries++) {
        // Send trigger pulse
        gpio_set_level(TRIG_PIN, 0);
        vTaskDelay(2 / portTICK_PERIOD_MS);
        gpio_set_level(TRIG_PIN, 1);
        esp_rom_delay_us(10);
        gpio_set_level(TRIG_PIN, 0);

        // Wait for echo to go high
        int64_t start_time = esp_timer_get_time();
        while (gpio_get_level(ECHO_PIN) == 0) {
            if (esp_timer_get_time() - start_time > 2000) {
                printf("Retrying... Echo signal not detected.\n");
                continue; // Retry
            }
        }

        // Measure the echo duration
        start_time = esp_timer_get_time();
        while (gpio_get_level(ECHO_PIN) == 1) {
            if (esp_timer_get_time() - start_time > 38000) {
                printf("Retrying... Echo signal timed out.\n");
                continue; // Retry
            }
        }

        int64_t echo_time = esp_timer_get_time() - start_time;

        // Calculate and return distance
        float speed_of_sound = get_speed_of_sound(temperature);
        float distance = (echo_time / 2.0) * (speed_of_sound / 1e6);
        return distance * 100; // Convert to cm
    }

    printf("Measurement failed after retries\n");
    return -1;
}


void app_main(void) {
    // Configure GPIO
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << TRIG_PIN);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << ECHO_PIN);
    gpio_config(&io_conf);

    float current_temperature = 25.0; // Replace with actual sensor reading if using a temperature sensor
    while (1) {
        float distance = measure_distance(current_temperature);
        if (distance > 0) {
            printf("Distance: %.2f cm at %.1f°C\n", distance, current_temperature);
        } else {
            printf("Failed to measure distance\n");
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS); // 1-second delay
    }
}

