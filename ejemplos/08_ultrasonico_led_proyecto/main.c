/*
 * 08_ultrasonico_led_proyecto/main.c - HC-SR04 con LED de aviso (ESP-IDF)
 * Monitoria: Introduccion al ESP32 - Intro. Ing. Electronica (UNAL)
 * Entorno: VS Code + extension ESP-IDF (IDF v5.x)
 * Concepto: medir el ancho de un pulso con esp_timer, umbral de distancia,
 *           bucle no bloqueante con vTaskDelay
 * Montaje: HC-SR04 con VCC a 5V, GND a GND, TRIG en GPIO18 y ECHO en GPIO19
 *          a traves de un divisor (1 kohm entre ECHO y el pin, 2 kohm del pin
 *          a GND): el ECHO del sensor entrega 5 V y el ESP32 solo aguanta 3.3 V.
 *          LED en GPIO21 con resistencia de 220 ohm a GND.
 *
 * pulseIn() de Arduino no existe en ESP-IDF: el ancho del pulso se mide
 * leyendo el pin y restando dos marcas de esp_timer_get_time().
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_rom_sys.h"

#define PIN_TRIG         GPIO_NUM_18
#define PIN_ECHO         GPIO_NUM_19
#define PIN_LED          GPIO_NUM_21

#define UMBRAL_CM        20.0f
#define INTERVALO_MS     100
#define TIMEOUT_US       30000    // ~5 m: si no hay eco se descarta la medida

// Devuelve true si hubo eco. La distancia se entrega en centimetros.
static bool medir_distancia(float *cm)
{
    // Pulso de 10 us en TRIG: el sensor responde con un pulso en ECHO
    gpio_set_level(PIN_TRIG, 0);
    esp_rom_delay_us(2);
    gpio_set_level(PIN_TRIG, 1);
    esp_rom_delay_us(10);
    gpio_set_level(PIN_TRIG, 0);

    // Espera el flanco de subida del eco
    int64_t t_espera = esp_timer_get_time();
    while (gpio_get_level(PIN_ECHO) == 0) {
        if (esp_timer_get_time() - t_espera > TIMEOUT_US) {
            return false;
        }
    }

    // Mide cuanto dura el pulso en alto
    int64_t inicio = esp_timer_get_time();
    while (gpio_get_level(PIN_ECHO) == 1) {
        if (esp_timer_get_time() - inicio > TIMEOUT_US) {
            return false;
        }
    }
    int64_t fin = esp_timer_get_time();

    // 58 us por cm: sonido a 343 m/s, ida y vuelta
    *cm = (float)(fin - inicio) / 58.0f;
    return true;
}

void app_main(void)
{
    gpio_set_direction(PIN_TRIG, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_ECHO, GPIO_MODE_INPUT);
    gpio_set_direction(PIN_LED, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_TRIG, 0);

    printf("Acerca la mano al sensor (umbral %d cm).\n", (int)UMBRAL_CM);

    while (1) {
        float distancia = 0.0f;

        if (!medir_distancia(&distancia)) {
            gpio_set_level(PIN_LED, 0);
            printf("Sin eco\n");
        } else {
            gpio_set_level(PIN_LED, distancia < UMBRAL_CM ? 1 : 0);
            printf("Distancia: %.1f cm%s\n",
                   distancia,
                   distancia < UMBRAL_CM ? "  <- LED encendido" : "");
        }

        vTaskDelay(pdMS_TO_TICKS(INTERVALO_MS));
    }
}
