/*
 * 02_semaforo/main.c - Semaforo de tres estados (ESP-IDF)
 * Monitoria: Introduccion al ESP32 - Intro. Ing. Electronica (UNAL)
 * Entorno: VS Code + extension ESP-IDF (IDF v5.x)
 * Concepto: varias salidas digitales, secuencia temporizada, funciones propias
 * Montaje: LED rojo en GPIO21, amarillo en GPIO22, verde en GPIO23 (220 ohm c/u)
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED_ROJO      GPIO_NUM_21
#define LED_AMARILLO  GPIO_NUM_22
#define LED_VERDE     GPIO_NUM_23

#define T_VERDE       3000
#define T_AMARILLO    1000
#define T_ROJO        3000

static void apagar_todos(void)
{
    gpio_set_level(LED_ROJO, 0);
    gpio_set_level(LED_AMARILLO, 0);
    gpio_set_level(LED_VERDE, 0);
}

void app_main(void)
{
    // Los tres pines se configuran de una sola vez con una mascara de bits.
    gpio_config_t salidas = {
        .pin_bit_mask = (1ULL << LED_ROJO) | (1ULL << LED_AMARILLO) | (1ULL << LED_VERDE),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&salidas);
    apagar_todos();

    while (1) {
        gpio_set_level(LED_VERDE, 1);
        vTaskDelay(pdMS_TO_TICKS(T_VERDE));
        gpio_set_level(LED_VERDE, 0);

        gpio_set_level(LED_AMARILLO, 1);
        vTaskDelay(pdMS_TO_TICKS(T_AMARILLO));
        gpio_set_level(LED_AMARILLO, 0);

        gpio_set_level(LED_ROJO, 1);
        vTaskDelay(pdMS_TO_TICKS(T_ROJO));
        gpio_set_level(LED_ROJO, 0);
    }
}
