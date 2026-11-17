/*
 * 13_oled_i2c.ino - Pantalla OLED SSD1306 128x64 por I2C
 * Monitoria: Introduccion al ESP32 - Intro. Ing. Electronica (UNAL)
 * Placa: ESP32-WROOM-32, ESP32-C6 o ESP32-S3
 * Concepto: bus I2C, pantalla de pixeles, buffer de Video, fuentes
 * Montaje: OLED VCC a 3V3, GND a GND, SDA y SCL segun la placa
 *
 * Librerias necesarias (Instalar > Librerias):
 *   - Adafruit GFX Library
 *   - Adafruit SSD1306
 *
 * Heltec WiFi Kit / LoRa 32 V3: SDA 17, SCL 18. Su OLED interna ya esta en
 * 0x3C: si conectas otra pantalla externa a la misma direccion chocan.
 *
 * OJO: el OLED es de 3.3 V. No lo alimentes con 5 V.
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------- PINES DEL BUS I2C ----------
#if defined(HELTEC_V3)
const uint8_t PIN_SDA = 17;           // Heltec V3: I2C de la pantalla interna
const uint8_t PIN_SCL = 18;
#elif defined(ARDUINO_ESP32C6_DEV) || defined(CONFIG_IDF_TARGET_ESP32C6)
const uint8_t PIN_SDA = 6;            // ESP32-C6 DevKitC-1
const uint8_t PIN_SCL = 7;
#elif defined(ARDUINO_ESP32S3_DEV) || defined(CONFIG_IDF_TARGET_ESP32S3)
const uint8_t PIN_SDA = 8;            // ESP32-S3 DevKit
const uint8_t PIN_SCL = 9;
#else
const uint8_t PIN_SDA = 21;           // ESP32-WROOM-32
const uint8_t PIN_SCL = 22;
#endif

const uint8_t ANCHO = 128;            // pixeles horizontales
const uint8_t ALTO  = 64;             // pixeles verticales
const uint8_t DIR_OLED = 0x3C;        // direccion tipica; algunos son 0x3D

// El ultimo parametro es el pin RESET. -1 = no usar pin de reset.
Adafruit_SSD1306 pantalla(ANCHO, ALTO, &Wire, -1);

void setup() {
    Wire.begin(PIN_SDA, PIN_SCL);

    Serial.begin(115200);

    if (!pantalla.begin(SSD1306_SWITCHCAPVCC, DIR_OLED)) {
        Serial.println(F("No se encontro el OLED. Revisa SDA/SCL y la direccion."));
        while (1);                    // se queda aqui: no tiene sentido seguir
    }

    pantalla.clearDisplay();          // borra el buffer de memoria
    pantalla.setTextSize(1);          // tamano de letra minimo (5x7 pixeles)
    pantalla.setTextColor(SSD1306_WHITE);

    pantalla.setCursor(0, 0);
    pantalla.print(F("HOLA DESDE ESP32"));
    pantalla.setCursor(0, 16);
    pantalla.print(F("OLED I2C 128x64"));

    pantalla.display();               // envia el buffer a la pantalla
}

void loop() {
    pantalla.fillRect(0, 40, 128, 9, SSD1306_BLACK);  // limpia solo esa franja

    pantalla.setCursor(0, 40);
    pantalla.print(F("TIEMPO: "));
    pantalla.print(millis() / 1000);
    pantalla.print(F(" s"));

    pantalla.display();               // sin esto no se ve ningun cambio
    delay(500);
}
