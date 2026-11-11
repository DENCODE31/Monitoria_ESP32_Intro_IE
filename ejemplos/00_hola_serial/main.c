/*
 * 00_hola_serial/main.c - Conocer el chip por el monitor serial (ESP-IDF)
 * Monitoria: Introduccion al ESP32 - Intro. Ing. Electronica (UNAL)
 * Entorno: VS Code + extension ESP-IDF (IDF v5.x)
 * Concepto: app_main(), printf(), esp_chip_info(), datos del heap y del SDK
 * Montaje: ninguno, solo el cable USB
 *
 * Como usarlo: crea un proyecto con la extension ESP-IDF (New Project),
 * reemplaza el contenido de main/main.c por este archivo y compila.
 * Monitor serial a 115200.
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "esp_idf_version.h"

// El IDF guarda el modelo como numero: aqui se traduce a texto legible.
static const char *nombre_chip(esp_chip_model_t modelo)
{
    switch (modelo) {
    case CHIP_ESP32:   return "ESP32";
    case CHIP_ESP32S2: return "ESP32-S2";
    case CHIP_ESP32S3: return "ESP32-S3";
    case CHIP_ESP32C3: return "ESP32-C3";
    case CHIP_ESP32C6: return "ESP32-C6";
    case CHIP_ESP32H2: return "ESP32-H2";
    default:           return "desconocido";
    }
}

void app_main(void)
{
    // app_main arranca solo: no hay setup(). Hay que dar tiempo al monitor serial.
    vTaskDelay(pdMS_TO_TICKS(1500));

    esp_chip_info_t info;
    esp_chip_info(&info);

    printf("\n========================================\n");
    printf("  Monitoria: Introduccion al ESP32\n");
    printf("========================================\n");

    printf("Modelo del chip : %s\n", nombre_chip(info.model));
    printf("Revision        : %d\n", info.revision);
    printf("Nucleos         : %d\n", info.cores);
    printf("Frecuencia      : %d MHz\n", CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ);
    printf("Heap libre      : %lu bytes\n", (unsigned long)esp_get_free_heap_size());
    printf("Version de IDF  : %s\n", esp_get_idf_version());
    printf("Target          : %s\n", CONFIG_IDF_TARGET);

    printf("----------------------------------------\n");
    printf("Si ves esto, el entorno esta listo.\n\n");

    // En ESP-IDF app_main debe terminar en un bucle infinito o con vTaskDelete(NULL).
    while (1) {
        printf("Activo hace %lu s\n", (unsigned long)(esp_timer_get_time() / 1000000ULL));
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
