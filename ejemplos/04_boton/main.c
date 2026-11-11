/*
 * 04_boton/main.c - Pulsador con antirrebote por software (ESP-IDF)
 * Monitoria: Introduccion al ESP32 - Intro. Ing. Electronica (UNAL)
 * Entorno: VS Code + extension ESP-IDF (IDF v5.x)
 * Concepto: entrada digital, pull-up interno, rebote, contador con millis()
 * Montaje: pulsador entre GPIO14 y GND, LED en GPIO21 con 220 ohm a GND
 *
 * Ojo: con pull-up la logica se invierte -> 0 significa presionado.
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"

#define BOTON          GPIO_NUM_14
#define LED            GPIO_NUM_21
#define DEBOUNCE_MS    30

static uint32_t millis(void)
{
    return (uint32_t)(esp_timer_get_time() / 1000ULL);
}

void app_main(void)
{
    // El ESP32 trae pull-up interno: no hace falta resistencia externa.
    gpio_config_t entrada = {
        .pin_bit_mask = 1ULL << BOTON,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&entrada);

    gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    printf("Presiona el boton (GPIO%d).\n", BOTON);

    int lectura_anterior = 1;
    uint32_t t_ultimo_cambio = 0;
    uint32_t contador = 0;

    while (1) {
        int lectura = gpio_get_level(BOTON);

        // Solo acepta el cambio si paso el tiempo de antirrebote
        if (lectura != lectura_anterior && (millis() - t_ultimo_cambio) > DEBOUNCE_MS) {
            t_ultimo_cambio = millis();
            lectura_anterior = lectura;

            if (lectura == 0) {               // flanco de bajada = pulsacion
                contador++;
                gpio_set_level(LED, 1);
                printf("Pulsaciones: %lu\n", (unsigned long)contador);
            } else {                          // flanco de subida = liberacion
                gpio_set_level(LED, 0);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(5));   // cede la CPU entre lecturas
    }
}
