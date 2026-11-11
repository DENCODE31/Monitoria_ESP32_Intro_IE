/*
 * 08_ultrasonico_led_proyecto/main.c - HC-SR04 con LED de aviso (ESP-IDF)
 * Monitoria: Introduccion al ESP32 - Intro. Ing. Electronica (UNAL)
 * Entorno: VS Code + extension ESP-IDF (IDF v5.x)
 * Concepto: medir el ancho de un pulso con esp_timer, umbral de distancia,
 *           bucle no bloqueante con vTaskDelay
 * Montaje: HC-SR04 con VCC a 5V, GND a GND, TRIG en GPIO18 y ECHO en GPIO19
 *          a traves de un divisor (1 kohm entre ECHO y el pin, 2 kohm del pin
 *          a GND): el ECHO del sensor entrega 5 V y el ESP32 solo aguanta 3.3 V.
 *          En ESP32-S3, GPIO19 es USB D-: si el montaje falla, mueve ECHO a GPIO4.
 *          LED en GPIO21 con resistencia de 220 ohm a GND.
 *
 * pulseIn() de Arduino no existe en ESP-IDF: el ancho del pulso se mide
 * leyendo el pin y restando dos marcas de esp_timer_get_time().
 */

#include <stdio.h>                    // printf: imprime en el monitor serial
#include <stdbool.h>                  // bool, true y false
#include "freertos/FreeRTOS.h"        // vTaskDelay: pausa sin bloquear todo
#include "freertos/task.h"            // pdMS_TO_TICKS: pasa ms a ticks
#include "driver/gpio.h"              // funciones de los pines
#include "esp_timer.h"                // reloj en microsegundos
#include "esp_rom_sys.h"              // esp_rom_delay_us: esperas de pocos us

// ---------- NUMEROS DE LOS PINES ----------
#if defined(HELTEC_V3)
#define PIN_TRIG         GPIO_NUM_47   // Heltec V3: GPIO18 es OLED SCL
#define PIN_ECHO         GPIO_NUM_48   // Heltec V3: GPIO19 es USB D-
#define PIN_LED          GPIO_NUM_4    // Heltec V3: GPIO21 es OLED_RST
#else
#define PIN_TRIG         GPIO_NUM_18   // TRIG: el ESP32 manda el disparo por aqui
#define PIN_ECHO         GPIO_NUM_19   // ECHO: el sensor responde por aqui
#define PIN_LED          GPIO_NUM_21   // LED: avisa que hay algo cerca
#endif

// ---------- VALORES QUE PODEMOS CAMBIAR ----------
#define UMBRAL_CM        20.0f         // si la distancia es menor a 20 cm, prende el LED
#define INTERVALO_MS     100           // espera 100 ms entre una medida y la siguiente
#define TIMEOUT_US       30000         // si el eco no llega en 30 ms, se descarta

// Mide la distancia. Devuelve true si hubo eco y deja el valor en *cm.
static bool medir_distancia(float *cm)
{
    // 1. el ESP32 manda un pulso de 10 microsegundos por TRIG
    gpio_set_level(PIN_TRIG, 0);      // baja TRIG antes de disparar
    esp_rom_delay_us(2);              // espera 2 us para asegurar el nivel bajo
    gpio_set_level(PIN_TRIG, 1);      // sube TRIG
    esp_rom_delay_us(10);             // lo mantiene arriba 10 us
    gpio_set_level(PIN_TRIG, 0);      // lo baja: el disparo ya salio

    // 2. espera a que ECHO suba (empieza la respuesta del sensor)
    int64_t t_espera = esp_timer_get_time();          // marca de tiempo en us
    while (gpio_get_level(PIN_ECHO) == 0) {           // mientras siga en 0...
        if (esp_timer_get_time() - t_espera > TIMEOUT_US) {
            return false;                             // se acabo el tiempo: sin eco
        }
    }

    // 3. mide cuanto tiempo se queda ECHO en 1
    int64_t inicio = esp_timer_get_time();            // instante en que subio
    while (gpio_get_level(PIN_ECHO) == 1) {           // mientras siga en 1...
        if (esp_timer_get_time() - inicio > TIMEOUT_US) {
            return false;                             // pulso demasiado largo: error
        }
    }
    int64_t fin = esp_timer_get_time();               // instante en que bajo

    // 4. pasa de tiempo a distancia: el sonido tarda 58 us en ir y volver 1 cm
    *cm = (float)(fin - inicio) / 58.0f;              // guarda el resultado en cm
    return true;                                      // si hubo eco
}

// app_main hace el papel del setup() y del loop() de Arduino
void app_main(void)
{
    gpio_set_direction(PIN_TRIG, GPIO_MODE_OUTPUT);   // TRIG manda senal: salida
    gpio_set_direction(PIN_ECHO, GPIO_MODE_INPUT);    // ECHO recibe senal: entrada
    gpio_set_direction(PIN_LED, GPIO_MODE_OUTPUT);    // LED se enciende y apaga: salida
    gpio_set_level(PIN_TRIG, 0);                      // TRIG empieza en 0

    printf("Acerca la mano al sensor (umbral %d cm).\n", (int)UMBRAL_CM);   // mensaje inicial

    // En ESP-IDF no hay loop(): el programa se queda dentro de while (1)
    while (1) {
        float distancia = 0.0f;                       // aqui se guarda el resultado

        if (!medir_distancia(&distancia)) {            // si no hubo eco...
            gpio_set_level(PIN_LED, 0);                // apaga el LED
            printf("Sin eco\n");                       // avisa en el monitor serial
        } else {                                       // si hubo eco...
            // cerca enciende (1), lejos apaga (0)
            gpio_set_level(PIN_LED, distancia < UMBRAL_CM ? 1 : 0);
            printf("Distancia: %.1f cm%s\n",           // imprime distancia con 1 decimal
                   distancia,                          // valor medido
                   distancia < UMBRAL_CM ? "  <- LED encendido" : "");   // aviso opcional
        }

        // pausa de 100 ms: le da tiempo al sensor y no bloquea el sistema
        vTaskDelay(pdMS_TO_TICKS(INTERVALO_MS));
    }
}
