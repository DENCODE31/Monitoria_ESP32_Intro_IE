/*
 * 05_potenciometro/main.c - Lectura analogica de 12 bits (ESP-IDF)
 * Monitoria: Introduccion al ESP32 - Intro. Ing. Electronica (UNAL)
 * Entorno: VS Code + extension ESP-IDF (IDF v5.x)
 * Concepto: driver ADC oneshot, resolucion 12 bits, escalado a PWM y a voltios
 * Montaje: potenciometro (extremos a 3V3 y GND, cursor al pin analogico)
 *          LED en GPIO21 con 220 ohm a GND
 *
 * El unico pin que cambia entre placas es el del ADC:
 *   WROOM-32 -> GPIO32 (ADC1_CH4)
 *   ESP32-C6 -> GPIO0  (ADC1_CH0)
 * En ESP-IDF no basta el GPIO: hay que indicar tambien el canal del ADC.
 * El target del proyecto define CONFIG_IDF_TARGET_ESP32C6 y elige solo.
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_timer.h"

#if defined(CONFIG_IDF_TARGET_ESP32C6)
#define PIN_ADC      0
#define CANAL_ADC    ADC_CHANNEL_0    // ESP32-C6 DevKitC-1: GPIO0 (ADC1_CH0)
#else
#define PIN_ADC      32
#define CANAL_ADC    ADC_CHANNEL_4    // ESP32-WROOM-32: GPIO32 (ADC1_CH4)
#endif

#define LED              GPIO_NUM_21
#define INTERVALO_MS     250
#define UMBRAL           2048      // mitad de 4095
#define FRECUENCIA_HZ    5000
#define RESOLUCION       LEDC_TIMER_8_BIT

static void pwm_init(void)
{
    ledc_timer_config_t temporizador = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = RESOLUCION,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = FRECUENCIA_HZ,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&temporizador);

    ledc_channel_config_t canal = {
        .gpio_num = LED,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0,
    };
    ledc_channel_config(&canal);
}

static void set_brillo(uint32_t duty)
{
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

void app_main(void)
{
    // ---- ADC en modo oneshot: una lectura bajo demanda ----
    adc_oneshot_unit_handle_t adc;
    adc_oneshot_unit_init_cfg_t config_unidad = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&config_unidad, &adc));

    adc_oneshot_chan_cfg_t config_canal = {
        .atten = ADC_ATTEN_DB_12,       // fondo de escala ~3.3 V
        .bitwidth = ADC_BITWIDTH_12,    // 0..4095, igual que en Arduino
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc, CANAL_ADC, &config_canal));

    pwm_init();

    printf("Gira el potenciometro (0-4095).\n");

    uint32_t t_ultimo = 0;

    while (1) {
        uint32_t ahora = (uint32_t)(esp_timer_get_time() / 1000ULL);

        if (ahora - t_ultimo < INTERVALO_MS) {
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }
        t_ultimo = ahora;

        int crudo = 0;
        ESP_ERROR_CHECK(adc_oneshot_read(adc, CANAL_ADC, &crudo));   // 0..4095

        float voltaje = crudo * (3.3f / 4095.0f);
        uint32_t brillo = (uint32_t)((crudo * 255) / 4095);          // 0..255

        set_brillo(brillo);

        printf("ADC=%d  V=%.2f V  PWM=%lu  LED=%s\n",
               crudo,
               voltaje,
               (unsigned long)brillo,
               crudo >= UMBRAL ? "ON" : "OFF");
    }
}
