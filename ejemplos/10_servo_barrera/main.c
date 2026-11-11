/*
 * 10_servo_barrera/main.c - HC-SR04 + servo: barrera que abre al acercarse
 * Monitoria: Introduccion al ESP32 - Intro. Ing. Electronica (UNAL)
 * Entorno: VS Code + extension ESP-IDF (IDF v5.x)
 * Concepto: sensor de distancia + actuador (servo), LEDC a 50 Hz, histeresis
 * Montaje: HC-SR04 con VCC a 5V, GND a GND, TRIG en GPIO18 y ECHO en GPIO19
 *          por divisor (1 kohm en serie, 2 kohm a GND).
 *          Servo de 5 V: senal en GPIO21, VCC a fuente de 5 V, GND comun.
 *
 * Heltec WiFi Kit / LoRa 32 V3: TRIG 47, ECHO 48, servo 5 (define HELTEC_V3).
 *
 * OJO: el servo NO se alimenta del 3V3 del ESP32. Consume picos de 500 mA.
 *      Usa el pin 5V de la placa o una fuente aparte con GND comun.
 *
 * Servo: pulso de 50 Hz (20 ms). 0.5 ms = 0 grados, 2.5 ms = 180 grados.
 */

#include <stdio.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_timer.h"
#include "esp_rom_sys.h"

// ---------- NUMEROS DE LOS PINES ----------
#if defined(HELTEC_V3)
#define PIN_TRIG         GPIO_NUM_47   // Heltec V3: GPIO18 es OLED SCL
#define PIN_ECHO         GPIO_NUM_48   // Heltec V3: GPIO19 es USB D-
#define PIN_SERVO        GPIO_NUM_5    // Heltec V3: GPIO21 es OLED_RST
#else
#define PIN_TRIG         GPIO_NUM_18   // TRIG: el ESP32 manda el disparo
#define PIN_ECHO         GPIO_NUM_19   // ECHO: el sensor responde
#define PIN_SERVO        GPIO_NUM_21   // senal del servo (PWM de 50 Hz)
#endif

// ---------- VALORES QUE PODEMOS CAMBIAR ----------
#define CERCA_CM         15.0f         // si algo esta a menos de 15 cm, abre
#define LEJOS_CM         20.0f         // si se aleja mas de 20 cm, cierra (histeresis)
#define INTERVALO_MS     100           // mide cada 100 ms
#define TIMEOUT_US       30000         // si no hay eco en 30 ms, se descarta

// ---------- ANGULOS DE LA BARRA ----------
#define ANGULO_CERRADO   0             // barra horizontal: paso bloqueado
#define ANGULO_ABIERTO   90            // barra levantada: paso libre

// ---------- PWM DEL SERVO ----------
#define SERVO_FREC_HZ    50            // el servo espera 50 pulsos por segundo
#define SERVO_RESOLUCION LEDC_TIMER_16_BIT   // 65536 pasos por periodo

static void servo_init(void)
{
    ledc_timer_config_t temporizador = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = SERVO_RESOLUCION,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = SERVO_FREC_HZ,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&temporizador);

    ledc_channel_config_t canal = {
        .gpio_num = PIN_SERVO,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0,
    };
    ledc_channel_config(&canal);
}

// Mueve el servo a un angulo entre 0 y 180 grados
static void mover_servo(int grados)
{
    // 0 grados = 500 us, 180 grados = 2500 us, repartidos linealmente
    int us = 500 + (grados * 2000) / 180;

    // El periodo es 20 ms = 20000 us, y el duty usa 65536 pasos
    uint32_t duty = (uint32_t)us * 65536UL / 20000UL;

    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

// Devuelve true si hubo eco. La distancia se entrega en centimetros.
static bool medir_distancia(float *cm)
{
    gpio_set_level(PIN_TRIG, 0);      // baja TRIG antes de disparar
    esp_rom_delay_us(2);
    gpio_set_level(PIN_TRIG, 1);      // pulso de 10 us
    esp_rom_delay_us(10);
    gpio_set_level(PIN_TRIG, 0);

    int64_t t_espera = esp_timer_get_time();          // espera el flanco de subida
    while (gpio_get_level(PIN_ECHO) == 0) {
        if (esp_timer_get_time() - t_espera > TIMEOUT_US) {
            return false;
        }
    }

    int64_t inicio = esp_timer_get_time();            // mide el pulso en alto
    while (gpio_get_level(PIN_ECHO) == 1) {
        if (esp_timer_get_time() - inicio > TIMEOUT_US) {
            return false;
        }
    }
    int64_t fin = esp_timer_get_time();

    *cm = (float)(fin - inicio) / 58.0f;              // 58 us por cm
    return true;
}

void app_main(void)
{
    servo_init();
    gpio_set_direction(PIN_TRIG, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_ECHO, GPIO_MODE_INPUT);
    gpio_set_level(PIN_TRIG, 0);

    mover_servo(ANGULO_CERRADO);      // arranca con la barra cerrada
    printf("Barrera lista. Acerca la mano al sensor.\n");

    bool abierta = false;

    while (1) {
        float distancia = 0.0f;

        if (!medir_distancia(&distancia)) {
            printf("Sin eco\n");
        } else {
            // Histeresis: abre cerca, cierra solo si se aleja de verdad
            if (!abierta && distancia < CERCA_CM) {
                abierta = true;
                mover_servo(ANGULO_ABIERTO);
            } else if (abierta && distancia > LEJOS_CM) {
                abierta = false;
                mover_servo(ANGULO_CERRADO);
            }

            printf("Distancia: %.1f cm  barrera=%s\n",
                   distancia,
                   abierta ? "ABIERTA" : "CERRADA");
        }

        vTaskDelay(pdMS_TO_TICKS(INTERVALO_MS));
    }
}
