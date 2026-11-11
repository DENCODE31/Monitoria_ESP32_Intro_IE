/*
 * 09_ldr_led_proyecto/main.c - LDR que enciende un LED al taparla (ESP-IDF)
 * Monitoria: Introduccion al ESP32 - Intro. Ing. Electronica (UNAL)
 * Entorno: VS Code + extension ESP-IDF (IDF v5.x)
 * Concepto: driver ADC oneshot, resolucion de 12 bits, umbral, vTaskDelay
 * Montaje: LDR entre 3V3 y el pin analogico, y resistencia de 10 kohm de ese
 *          pin a GND (divisor de voltaje). LED en GPIO21 con 220 ohm a GND.
 *
 *   WROOM-32 -> LDR en GPIO32 (ADC1_CH4)
 *   ESP32-C6 -> LDR en GPIO0  (ADC1_CH0)
 * Ojo: el ESP32 no aguanta 5 V en el pin analogico. El divisor va a 3V3.
 *
 * En Arduino basta analogRead(). En ESP-IDF hay que crear una unidad ADC,
 * decirle en que canal esta el pin y pedir cada lectura una por una.
 */

#include <stdio.h>                        // printf: imprime en el monitor serial
#include <stdbool.h>                      // bool, true y false
#include "freertos/FreeRTOS.h"            // vTaskDelay: pausa sin bloquear todo
#include "freertos/task.h"                // pdMS_TO_TICKS: pasa ms a ticks
#include "driver/gpio.h"                  // funciones de los pines
#include "esp_adc/adc_oneshot.h"          // driver del ADC
#include "esp_timer.h"                    // reloj en milisegundos

// ---------- PIN Y CANAL DEL ADC ----------
// El target del proyecto define CONFIG_IDF_TARGET_ESP32C6 y elige solo.
#if defined(CONFIG_IDF_TARGET_ESP32C6)
#define PIN_ADC       0                 // ESP32-C6: GPIO0 es entrada analogica
#define CANAL_ADC     ADC_CHANNEL_0     // y ese GPIO pertenece al canal 0
#else
#define PIN_ADC       32                // ESP32-WROOM-32: GPIO32 es entrada analogica
#define CANAL_ADC     ADC_CHANNEL_4     // y ese GPIO pertenece al canal 4
#endif

// ---------- NUMEROS DE LOS PINES ----------
#define PIN_LED       GPIO_NUM_21       // LED: se enciende cuando hay poca luz

// ---------- VALORES QUE PODEMOS CAMBIAR ----------
// Si la lectura baja de este valor, se prende el LED.
// Calibren: pongan el valor a mitad de camino entre la lectura con luz y tapada.
#define UMBRAL        800               // sobre 0..4095 (el equivalente a 200 de 0..1023)

// ---------- VALORES FIJOS ----------
#define INTERVALO_MS  100               // lee el sensor cada 100 ms

// app_main hace el papel del setup() y del loop() de Arduino
void app_main(void)
{
    // ---- 1. crear la unidad del ADC (el bloque que mide) ----
    adc_oneshot_unit_handle_t adc;                    // nombre para usar el ADC
    adc_oneshot_unit_init_cfg_t config_unidad = {
        .unit_id = ADC_UNIT_1,                        // ADC1: sigue funcionando con Wi-Fi
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&config_unidad, &adc));   // si falla, avisa

    // ---- 2. decirle al ADC en que canal esta el LDR ----
    adc_oneshot_chan_cfg_t config_canal = {
        .atten = ADC_ATTEN_DB_12,                     // fondo de escala ~3.3 V
        .bitwidth = ADC_BITWIDTH_12,                  // lectura de 0 a 4095
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc, CANAL_ADC, &config_canal));

    // ---- 3. configurar el LED y avisar por el monitor ----
    gpio_set_direction(PIN_LED, GPIO_MODE_OUTPUT);    // el LED es una salida
    gpio_set_level(PIN_LED, 0);                       // empieza apagado
    printf("Tapa el LDR: la lectura baja y el LED se enciende.\n");

    uint32_t t_ultimo = 0;                            // instante de la ultima lectura

    // En ESP-IDF no hay loop(): el programa se queda dentro de while (1)
    while (1) {
        // esp_timer_get_time() da microsegundos: se divide para tener milisegundos
        uint32_t ahora = (uint32_t)(esp_timer_get_time() / 1000ULL);

        if (ahora - t_ultimo < INTERVALO_MS) {        // todavia no pasan 100 ms...
            vTaskDelay(pdMS_TO_TICKS(10));            // espera un poco sin bloquear
            continue;                                 // vuelve a preguntar por el tiempo
        }
        t_ultimo = ahora;                             // guarda este instante como el ultimo

        // ---- 4. leer el LDR: con mucha luz el valor es alto, tapada es bajo ----
        int luz = 0;                                  // aqui se guarda la lectura
        ESP_ERROR_CHECK(adc_oneshot_read(adc, CANAL_ADC, &luz));   // 0 a 4095

        // ---- 5. comparar con el umbral: true = poca luz ----
        bool poca_luz = (luz < UMBRAL);

        // ---- 6. prender el LED solo cuando hay poca luz ----
        gpio_set_level(PIN_LED, poca_luz ? 1 : 0);    // ? : es un if corto

        // ---- 7. mostrar la lectura y el estado en el monitor serial ----
        printf("luz (0-4095): %d  LED=%s\n",           // etiqueta y separador
               luz,                                   // valor medido
               poca_luz ? "ON" : "OFF");              // estado del LED
    }
}
