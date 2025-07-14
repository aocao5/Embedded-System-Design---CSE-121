#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#define TAG "MORSE_RECEIVER"

// ADC Configuration
#define ADC_UNIT                ADC_UNIT_1
#define ADC_CHANNEL             ADC_CHANNEL_4 // Adjust to match your hardware
#define ADC_ATTEN               ADC_ATTEN_DB_12
#define THRESHOLD               1500          // Adjust based on photodiode signal

// Morse Code Timing (in ms)
#define DOT_DURATION_MS         200
#define DASH_DURATION_MS        600
#define SYMBOL_GAP_MS           400
#define LETTER_GAP_MS           800
#define WORD_GAP_MS             1200

// Morse Code Mapping
static const char *MORSE_CODE[] = {
    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---",
    "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-",
    "..-", "...-", ".--", "-..-", "-.--", "--..",  // A-Z
    "-----", ".----", "..---", "...--", "....-", ".....", "-....", "--...",
    "---..", "----."  // 0-9
};

// Function to decode a Morse code symbol to a character
static char decode_morse(const char *symbol) {
    for (int i = 0; i < sizeof(MORSE_CODE) / sizeof(MORSE_CODE[0]); i++) {
        if (strcmp(symbol, MORSE_CODE[i]) == 0) {
            if (i < 26) return 'A' + i; // Decode letters
            return '0' + (i - 26); // Decode numbers
        }
    }
    return '?'; // Unknown symbol
}

void app_main(void) {
    // ADC oneshot initialization
    adc_oneshot_unit_handle_t adc_handle;
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle));

    // ADC channel configuration
    adc_oneshot_chan_cfg_t channel_config = {
        .atten = ADC_ATTEN,
        .bitwidth = ADC_BITWIDTH_12,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, ADC_CHANNEL, &channel_config));

    // Variables for Morse code decoding
    char morse_buffer[16] = {0};
    int symbol_index = 0;
    TickType_t start_time = 0;
    bool signal_active = false;

    ESP_LOGI(TAG, "Morse Receiver Started");

    while (1) {
        int adc_value = 0;
        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL, &adc_value));
        ESP_LOGI(TAG, "ADC Value: %d", adc_value);

        if (adc_value > THRESHOLD) {
            if (!signal_active) {
                start_time = xTaskGetTickCount();
                signal_active = true;
            }
        } else {
            if (signal_active) {
                TickType_t duration = xTaskGetTickCount() - start_time;
                signal_active = false;

                if (duration < pdMS_TO_TICKS(DOT_DURATION_MS)) {
                    morse_buffer[symbol_index++] = '.';
                } else if (duration < pdMS_TO_TICKS(DASH_DURATION_MS)) {
                    morse_buffer[symbol_index++] = '-';
                } else if (duration >= pdMS_TO_TICKS(LETTER_GAP_MS)) {
                    morse_buffer[symbol_index] = '\0';
                    char decoded_char = decode_morse(morse_buffer);
                    printf("%c", decoded_char);
                    fflush(stdout);
                    symbol_index = 0; // Reset for next symbol
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Small delay to stabilize readings
    }

    // Clean up resources
    ESP_ERROR_CHECK(adc_oneshot_del_unit(adc_handle));
}

