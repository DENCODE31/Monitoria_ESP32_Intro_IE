/*
 * 03_pwm/main.c - Intensidad variable con PWM (efecto respiracion, ESP-IDF)
 * Monitoria: Introduccion al ESP32 - Intro. Ing. Electronica (UNAL)
 * Entorno: VS Code + extension ESP-IDF (IDF v5.x)
 * Concepto: LEDC (el periferico PWM del ESP32), duty de 8 bits, cambio de
 *           direccion con un contador de tiempo no bloqueante
 * Montaje: LED en GPIO21 con resistencia de 220 ohm a GND
 *
 * En Arduino bastaba analogWrite(). En ESP-IDF hay que configurar primero
 * un temporizador (la frecuencia) y un canal (el pin).
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_timer.h"

#if defined(HELTEC_V3)
#define LED              GPIO_NUM_4    // Heltec WiFi LoRa 32 V3: GPIO4 libre
#else
#define LED              GPIO_NUM_21
#endif
#define PASO             3
#define INTERVALO_MS     15
#define FRECUENCIA_HZ    5000      // 5 kHz: muy por encima del parpadeo visible
#define RESOLUCION       LEDC_TIMER_8_BIT   // duty de 0 a 255, igual que Arduino

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

// Equivalente a analogWrite(LED, brillo)
static void set_brillo(uint32_t duty)
{
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

void app_main(void)
{
    pwm_init();

    int brillo = 0;
    int direccion = 1;
    uint32_t t_ultimo = 0;

    while (1) {
        uint32_t ahora = (uint32_t)(esp_timer_get_time() / 1000ULL);   // millis()

        if (ahora - t_ultimo < INTERVALO_MS) {
            vTaskDelay(pdMS_TO_TICKS(1));   // cede la CPU: obligatorio en FreeRTOS
            continue;
        }
        t_ultimo = ahora;

        brillo += direccion * PASO;

        if (brillo >= 255) {              // techo: empieza a bajar
            brillo = 255;
            direccion = -1;
        } else if (brillo <= 0) {         // piso: empieza a subir
            brillo = 0;
            direccion = 1;
        }

        set_brillo((uint32_t)brillo);
    }
}
