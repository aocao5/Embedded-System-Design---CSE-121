#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/gpio.h"

#define TAG "MORSE_RECEIVER"

// ADC Configuration
#define ADC_UNIT            ADC_UNIT_1
#define ADC_CHANNEL         ADC_CHANNEL_4
#define ADC_ATTEN           ADC_ATTEN_DB_12
#define THRESHOLD           50 // Adjusted for testing

// Digital GPIO Configuration
#define SIGNAL_GPIO         GPIO_NUM_21 // Use any available GPIO pin
#define GPIO_INPUT_PIN_SEL  (1ULL << SIGNAL_GPIO)

// Speed Measurement
#define TEST_DURATION_MS    5000 // Test duration for speed in ms

// Morse Code Mapping
static const char *MORSE_CODE[] = {
    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---",
    "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-",
    "..-", "...-", ".--", "-..-", "-.--", "--..",  // A-Z
    "-----", ".----", "..---", "...--", "....-", ".....", "-....", "--...",
    "---..", "----."  // 0-9
};

// Function to decode Morse symbols to characters
static char decode_morse(const char *symbol) {
    for (int i = 0; i < sizeof(MORSE_CODE) / sizeof(MORSE_CODE[0]); i++) {
        if (strcmp(symbol, MORSE_CODE[i]) == 0) {
            if (i < 26) return 'A' + i; // Decode letters
            return '0' + (i - 26);     // Decode numbers
        }
    }
    return '?'; // Unknown symbol
}

void app_main(void) {
    adc_oneshot_unit_handle_t adc_handle;

    // Initialize ADC
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle));

    // Configure ADC Channel
    adc_oneshot_chan_cfg_t channel_config = {
        .atten = ADC_ATTEN,
        .bitwidth = ADC_BITWIDTH_12,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, ADC_CHANNEL, &channel_config));

    // Configure Digital GPIO
    gpio_config_t io_conf = {
        .pin_bit_mask = GPIO_INPUT_PIN_SEL,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);

    ESP_LOGI(TAG, "Morse Receiver with Speed Measurement Started");

    uint32_t character_count = 0;
    int pass_speed = 0, fail_speed = 0;

    // Speed Test Loop
    for (int speed = 5; speed <= 100; speed += 1) { // Test speeds from 5 to 100 characters/second
        character_count = 0;
        uint32_t start_time = xTaskGetTickCount() * portTICK_PERIOD_MS;

        while ((xTaskGetTickCount() * portTICK_PERIOD_MS) - start_time < TEST_DURATION_MS) {
            int adc_value = 0;
            ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL, &adc_value));
            int signal = gpio_get_level(SIGNAL_GPIO);

            if (adc_value > THRESHOLD || signal == 1) {
                character_count++;
                ESP_LOGI(TAG, "Signal HIGH Detected at Speed: %d characters/second", speed);
            }
            vTaskDelay(pdMS_TO_TICKS(1000 / speed)); // Delay adjusted for the current speed
        }

        float actual_speed = (float)character_count / (TEST_DURATION_MS / 1000);
        ESP_LOGI(TAG, "Tested Speed: %d characters/second, Measured Speed: %.2f", speed, actual_speed);

        // Determine Pass/Fail
        if (actual_speed >= speed) {
            pass_speed = speed;
        } else {
            fail_speed = speed;
            break;
        }
    }

    float difference = (float)(fail_speed - pass_speed) / pass_speed;
    if (difference <= 0.25) {
        ESP_LOGI(TAG, "Pass Speed: %d, Fail Speed: %d, Difference: %.2f%%", pass_speed, fail_speed, difference * 100);
    } else {
        ESP_LOGW(TAG, "Difference exceeds 25%%. Please refine the test.");
    }

    // Clean up resources
    ESP_ERROR_CHECK(adc_oneshot_del_unit(adc_handle));
}

