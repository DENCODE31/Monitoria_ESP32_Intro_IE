/*
 * 01_blink/main.c - Parpadeo de un LED externo (ESP-IDF)
 * Monitoria: Introduccion al ESP32 - Intro. Ing. Electronica (UNAL)
 * Entorno: VS Code + extension ESP-IDF (IDF v5.x)
 * Concepto: app_main(), gpio_set_direction(), gpio_set_level(), vTaskDelay()
 * Montaje: LED en GPIO21 con resistencia de 220 ohm a GND
 *
 * Nota: no usamos el LED integrado porque no es igual en ambas placas.
 *   WROOM-32 DevKit V1  -> LED simple en GPIO2
 *   ESP32-C6 DevKitC-1  -> LED RGB WS2812 en GPIO8 (necesita driver RGB)
 * Con un LED externo en GPIO21 el mismo montaje y codigo sirven en las dos.
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED_PIN      GPIO_NUM_21
#define PERIODO_MS   500

void app_main(void)
{
    // En Arduino esto era pinMode(LED_PIN, OUTPUT)
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    while (1) {
        gpio_set_level(LED_PIN, 1);              // 3.3 V -> LED encendido
        vTaskDelay(pdMS_TO_TICKS(PERIODO_MS));   // reemplaza delay()
        gpio_set_level(LED_PIN, 0);              // 0 V -> LED apagado
        vTaskDelay(pdMS_TO_TICKS(PERIODO_MS));
    }
}
