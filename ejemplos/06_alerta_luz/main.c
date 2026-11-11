/*
 * 06_alerta_luz/main.c - Integrador: sensor LDR + LED + buzzer (ESP-IDF)
 * Monitoria: Introduccion al ESP32 - Intro. Ing. Electronica (UNAL)
 * Entorno: VS Code + extension ESP-IDF (IDF v5.x)
 * Concepto: ADC oneshot, umbral con histeresis, actuadores, PWM para el buzzer
 * Montaje: LDR entre 3V3 y el pin analogico, resistencia de 10 kohm de ese
 *          pin a GND. LED en GPIO21 (220 ohm a GND). Buzzer pasivo en GPIO22.
 *
 *   WROOM-32 -> LDR en GPIO32
 *   ESP32-C6 -> LDR en GPIO0
 *
 * tone() de Arduino no existe en ESP-IDF: el buzzer pasivo se genera con LEDC
 * a una frecuencia fija y duty del 50 %.
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
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
#define PIN_ADC      1
#define CANAL_ADC    ADC_CHANNEL_0    // ESP32-S3 DevKitC-1: GPIO1 (ADC1_CH0)
#else
#define PIN_ADC      32
#define CANAL_ADC    ADC_CHANNEL_4    // ESP32-WROOM-32: GPIO32 (ADC1_CH4)
#endif

#define LED              GPIO_NUM_21
#define BUZZER           GPIO_NUM_22
#define CANAL_BUZZER     LEDC_CHANNEL_1
#define TIMER_BUZZER     LEDC_TIMER_1
#define FREC_BUZZER_HZ   1200      // buzzer pasivo: necesita frecuencia
#define RESOLUCION       LEDC_TIMER_8_BIT

#define UMBRAL           2000      // calibrar con el monitor serial
#define HISTERESIS       150       // evita el titileo cerca del umbral
#define INTERVALO_MS     200

static void buzzer_init(void)
{
    ledc_timer_config_t temporizador = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = RESOLUCION,
        .timer_num = TIMER_BUZZER,
        .freq_hz = FREC_BUZZER_HZ,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&temporizador);

    ledc_channel_config_t canal = {
        .gpio_num = BUZZER,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = CANAL_BUZZER,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = TIMER_BUZZER,
        .duty = 0,                 // 0 = sonando apagado
        .hpoint = 0,
    };
    ledc_channel_config(&canal);
}

// Equivalente a tone() / noTone()
static void buzzer(bool encendido)
{
    // duty 128 de 255 = 50 %: la onda cuadrada mas fuerte para el buzzer
    ledc_set_duty(LEDC_LOW_SPEED_MODE, CANAL_BUZZER, encendido ? 128 : 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, CANAL_BUZZER);
}

void app_main(void)
{
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    buzzer_init();

    adc_oneshot_unit_handle_t adc;
    adc_oneshot_unit_init_cfg_t config_unidad = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&config_unidad, &adc));

    adc_oneshot_chan_cfg_t config_canal = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc, CANAL_ADC, &config_canal));

    printf("Tapa el LDR para activar la alarma.\n");

    bool alarma = false;
    uint32_t t_ultimo = 0;

    while (1) {
        uint32_t ahora = (uint32_t)(esp_timer_get_time() / 1000ULL);

        if (ahora - t_ultimo < INTERVALO_MS) {
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }
        t_ultimo = ahora;

        int luz = 0;
        ESP_ERROR_CHECK(adc_oneshot_read(adc, CANAL_ADC, &luz));   // oscuro -> valor bajo

        // Histeresis: enciende por debajo de UMBRAL-H, apaga por encima de UMBRAL+H
        if (!alarma && luz < (UMBRAL - HISTERESIS)) {
            alarma = true;
        } else if (alarma && luz > (UMBRAL + HISTERESIS)) {
            alarma = false;
        }

        gpio_set_level(LED, alarma ? 1 : 0);
        buzzer(alarma);

        printf("Luz=%d  Alarma=%s\n", luz, alarma ? "ON" : "OFF");
    }
}
