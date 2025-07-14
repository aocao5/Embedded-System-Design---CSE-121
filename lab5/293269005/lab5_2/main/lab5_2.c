#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"

#define TAG "MORSE_RECEIVER"

// ADC Configuration
#define ADC_UNIT            ADC_UNIT_1
#define ADC_CHANNEL         ADC_CHANNEL_4
#define ADC_ATTEN           ADC_ATTEN_DB_12
#define THRESHOLD   50 // Temporarily lower threshold for testing

// Morse Code Timing (in ms)
#define DOT_DURATION_MS     200
#define DASH_DURATION_MS    600
#define SYMBOL_GAP_MS       400
#define LETTER_GAP_MS       800
#define WORD_GAP_MS         1200

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

    ESP_LOGI(TAG, "Morse Receiver Started");

    // Test ADC Values on Initialization
    for (int i = 0; i < 10; i++) {
        int adc_value = 0;
        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL, &adc_value));
        ESP_LOGI(TAG, "Initial ADC Test Value: %d", adc_value);
        vTaskDelay(pdMS_TO_TICKS(1000)); // 1-second delay between tests
    }

    while (1) {
        int adc_value = 0;
        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL, &adc_value));
        ESP_LOGI(TAG, "ADC Value: %d", adc_value);

        if (adc_value > THRESHOLD) {
            ESP_LOGI(TAG, "Signal HIGH Detected. ADC Value: %d\n", adc_value);
			//decode_morse(adc_value);
        } else {
            ESP_LOGI(TAG, "Signal LOW Detected. ADC Value: %d\n", adc_value);
        }

        vTaskDelay(pdMS_TO_TICKS(500)); // Adjust delay to control sampling rate
    }

    // Clean up resources
    ESP_ERROR_CHECK(adc_oneshot_del_unit(adc_handle));
}

