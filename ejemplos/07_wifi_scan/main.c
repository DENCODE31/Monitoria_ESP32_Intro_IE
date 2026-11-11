/*
 * 07_wifi_scan/main.c - Escaneo de redes Wi-Fi (ESP-IDF)
 * Monitoria: Introduccion al ESP32 - Intro. Ing. Electronica (UNAL)
 * Entorno: VS Code + extension ESP-IDF (IDF v5.x)
 * Concepto: radio Wi-Fi integrada, primer contacto con esp_wifi
 * Montaje: ninguno
 *
 * En Arduino son 3 lineas (WiFi.scanNetworks). En ESP-IDF hay que inicializar
 * NVS, la pila de red y el driver Wi-Fi antes de escanear. Este ejemplo se
 * copia del repositorio, no se escribe en clase.
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_timer.h"

#define INTERVALO_MS     15000    // escanea cada 15 s
#define MAX_REDES        20
#define CANAL_MIN        1
#define CANAL_MAX        13

// Fuera del stack: 20 registros no caben comodos en la tarea principal.
static wifi_ap_record_t redes[MAX_REDES];

static void wifi_init(void)
{
    // NVS guarda la calibracion de la radio: el driver no arranca sin ella.
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&config));

    // Modo estacion (cliente), igual que WiFi.mode(WIFI_STA) en Arduino
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_disconnect());
}

int escanear_redes(void)
{
    uint16_t encontradas = 0;

    wifi_scan_config_t config = {
        .ssid = NULL,
        .bssid = NULL,
        .channel = 0,              // 0 = todos los canales
        .show_hidden = false,
        .scan_type = WIFI_SCAN_TYPE_ACTIVE,
    };

    printf("Escaneando...\n");
    ESP_ERROR_CHECK(esp_wifi_scan_start(&config, true));   // true = espera a terminar

    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&encontradas));
    if (encontradas == 0) {
        printf("No se encontraron redes.\n");
        esp_wifi_clear_ap_list();
        return 0;
    }

    if (encontradas > MAX_REDES) {
        encontradas = MAX_REDES;
    }
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&encontradas, redes));

    printf("%u redes encontradas:\n", (unsigned)encontradas);
    printf("  #  SSID                             RSSI  Canal  Cifrado\n");

    for (int i = 0; i < encontradas; i++) {
        printf("  %d  %-32s %4d  %5d  %s\n",
               i + 1,
               (char *)redes[i].ssid,
               redes[i].rssi,
               redes[i].primary,
               redes[i].authmode == WIFI_AUTH_OPEN ? "abierta" : "protegida");
    }

    esp_wifi_clear_ap_list();      // libera la memoria del escaneo
    printf("\n");

    return encontradas;
}

void app_main(void)
{
    wifi_init();
    printf("Escaneo Wi-Fi iniciado.\n");

    while (1) {
        escanear_redes();
        vTaskDelay(pdMS_TO_TICKS(INTERVALO_MS));
    }
}
