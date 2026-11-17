/*
 * 12_lcd_i2c/main.c - Pantalla LCD 16x2 con modulo I2C (ESP-IDF)
 * Monitoria: Introduccion al ESP32 - Intro. Ing. Electronica (UNAL)
 * Entorno: VS Code + extension ESP-IDF (IDF v5.3+ / v6.x)
 * Concepto: bus I2C maestro, expandidor PCF8574, HD44780 en modo 4 bits
 * Montaje: modulo LCD I2C con VCC a 3V3, GND a GND, SDA y SCL segun la placa
 * Direccion: 0x27 (algunos modulos usan 0x3F)
 *
 * No se necesita ningun componente externo: el control del HD44780 y del
 * PCF8574 esta escrito aqui para que se vea que hace cada libreria.
 *
 * Si el enlazado no encuentra i2c_new_master_bus, agrega REQUIRES esp_driver_i2c
 * en main/CMakeLists.txt.
 *
 * OJO: si el modulo tiene pull-ups a 5 V, alimentalo desde 3V3 o usa un
 * level shifter. Un SDA/SCL a 5 V puede danar el ESP32.
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "esp_rom_sys.h"

// ---------- PINES DEL BUS I2C ----------
#if defined(HELTEC_V3)
#define PIN_SDA          17    // Heltec V3: I2C de la pantalla interna
#define PIN_SCL          18
#elif defined(CONFIG_IDF_TARGET_ESP32C6)
#define PIN_SDA          6     // ESP32-C6 DevKitC-1
#define PIN_SCL          7
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
#define PIN_SDA          8     // ESP32-S3 DevKit
#define PIN_SCL          9
#else
#define PIN_SDA          21    // ESP32-WROOM-32
#define PIN_SCL          22
#endif

#define DIR_LCD          0x27
#define FREC_I2C_HZ      100000

// ---------- BITS DEL PCF8574 ----------
// El PCF8574 tiene 8 salidas. El modulo LCD conecta:
// P4..P7 = D4..D7 del HD44780, P0 = RS, P1 = RW, P2 = E, P3 = backlight
#define LCD_RS           0x01
#define LCD_RW           0x02
#define LCD_EN           0x04
#define LCD_BACKLIGHT    0x08

static i2c_master_dev_handle_t lcd = NULL;

// Envia un byte al PCF8574 con la luz de fondo encendida
static void lcd_escribir(uint8_t dato)
{
    uint8_t buf[1] = { dato | LCD_BACKLIGHT };
    i2c_master_transmit(lcd, buf, 1, 100);
}

// Genera el pulso de habilitacion (E) que lee el HD44780
static void lcd_pulso(uint8_t dato)
{
    lcd_escribir(dato | LCD_EN);       // E en alto
    esp_rom_delay_us(450);
    lcd_escribir(dato & ~LCD_EN);      // E en bajo: aqui lee el HD44780
    esp_rom_delay_us(100);
}

// Envia un nibble de 4 bits con RS=0 (comando) o RS=1 (caracter)
static void lcd_nibble(uint8_t nibble, uint8_t rs)
{
    uint8_t dato = ((nibble & 0x0F) << 4) | rs | LCD_BACKLIGHT;
    lcd_pulso(dato);
}

// Envia un comando de 8 bits como dos nibbles
static void lcd_comando(uint8_t comando)
{
    lcd_nibble(comando >> 4, 0);       // nibble alto
    lcd_nibble(comando, 0);            // nibble bajo

    // Borrar pantalla y volver al inicio tardan mas que un comando normal
    if (comando == 0x01 || comando == 0x02) {
        vTaskDelay(pdMS_TO_TICKS(2));
    }
}

static void lcd_caracter(char c)
{
    lcd_nibble((uint8_t)c >> 4, LCD_RS);
    lcd_nibble((uint8_t)c, LCD_RS);
}

// Coloca el cursor: columna 0..15, fila 0..1
static void lcd_cursor(uint8_t columna, uint8_t fila)
{
    uint8_t direccion = columna + (fila == 1 ? 0x40 : 0x00);
    lcd_comando(0x80 | direccion);     // 0x80 = comando "poner cursor"
}

static void lcd_texto(const char *texto)
{
    for (size_t i = 0; texto[i] != '\0'; i++) {
        lcd_caracter(texto[i]);
    }
}

static void lcd_inicializar(void)
{
    vTaskDelay(pdMS_TO_TICKS(50));     // espera a que la LCD se estabilice
    lcd_escribir(0x00);                // PCF8574 con salidas en bajo

    // Secuencia de arranque del HD44780: tres veces 0x03 y luego 0x02
    lcd_nibble(0x03, 0);
    vTaskDelay(pdMS_TO_TICKS(5));
    lcd_nibble(0x03, 0);
    esp_rom_delay_us(150);
    lcd_nibble(0x03, 0);
    esp_rom_delay_us(150);
    lcd_nibble(0x02, 0);               // ya en modo de 4 bits

    lcd_comando(0x28);                 // 2 filas, caracteres de 5x8
    lcd_comando(0x0C);                 // pantalla encendida, cursor apagado
    lcd_comando(0x06);                 // escribe hacia la derecha
    lcd_comando(0x01);                 // borra la pantalla
}

void app_main(void)
{
    // 1. Crear el bus I2C maestro
    i2c_master_bus_config_t config_bus = {
        .i2c_port = I2C_NUM_0,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .scl_io_num = PIN_SCL,
        .sda_io_num = PIN_SDA,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    i2c_master_bus_handle_t bus = NULL;
    i2c_new_master_bus(&config_bus, &bus);

    // 2. Agregar el PCF8574 al bus
    i2c_device_config_t config_lcd = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = DIR_LCD,
        .scl_speed_hz = FREC_I2C_HZ,
    };
    i2c_master_bus_add_device(bus, &config_lcd, &lcd);

    // 3. Inicializar y escribir el texto fijo
    lcd_inicializar();
    lcd_cursor(0, 0);
    lcd_texto("HOLA DESDE IDF");
    lcd_cursor(0, 1);
    lcd_texto("I2C LCD 16x2");

    printf("LCD I2C lista.\n");

    uint32_t segundos = 0;

    while (1) {
        char linea[17];
        snprintf(linea, sizeof(linea), "TIEMPO: %lu s   ", segundos);

        lcd_cursor(0, 1);
        lcd_texto(linea);              // los espacios borran digitos sobrantes

        segundos++;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
