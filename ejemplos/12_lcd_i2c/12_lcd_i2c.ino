/*
 * 12_lcd_i2c.ino - Pantalla LCD 16x2 con modulo I2C (PCF8574)
 * Monitoria: Introduccion al ESP32 - Intro. Ing. Electronica (UNAL)
 * Placa: ESP32-WROOM-32, ESP32-C6 o ESP32-S3
 * Concepto: bus I2C, direcciones de dispositivo, LCD HD44780 en modo 4 bits
 * Conexiones del modulo LCD I2C:
 *   VCC -> 3V3
 *   GND -> GND
 *   SDA -> GPIO21 (WROOM-32) / GPIO6 (C6) / GPIO8 (S3) / GPIO17 (Heltec V3)
 *   SCL -> GPIO22 (WROOM-32) / GPIO7 (C6) / GPIO9 (S3) / GPIO18 (Heltec V3)
 *   Direccion I2C: 0x27 (algunos modulos usan 0x3F)
 *
 * Libreria necesaria: LiquidCrystal_I2C (Instalar > Librerias > buscar
 * "LiquidCrystal_I2C" > instalar la de John Rickman).
 *
 * Heltec WiFi Kit / LoRa 32 V3: SDA 17, SCL 18 (OLED interno en 0x3C).
 *
 * OJO: muchos modulos traen pull-ups a 5 V. Si alimentas el modulo con 5 V,
 * el SDA/SCL llega por encima de los 3.3 V del ESP32. Alimenta el modulo
 * desde 3V3, o usa un level shifter.
 *
 * Si no ves texto, ajusta el contraste con el potenciometro del modulo.
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

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

const uint8_t DIR_LCD = 0x27;         // direccion comun; algunos modulos son 0x3F
LiquidCrystal_I2C lcd(DIR_LCD, 16, 2);  // direccion, columnas, filas

void setup() {
    Wire.begin(PIN_SDA, PIN_SCL);     // arranca el bus I2C en estos pines

    lcd.init();                       // inicializa el controlador HD44780
    lcd.backlight();                  // enciende la luz de fondo
    lcd.clear();

    lcd.setCursor(0, 0);              // columna 0, fila 0 (fila superior)
    lcd.print("HOLA DESDE ESP32");   // <- escribe aqui el mensaje de la fila 0
    lcd.setCursor(0, 1);              // columna 0, fila 1 (fila inferior)
    lcd.print("I2C LCD 16x2");       // <- escribe aqui el mensaje de la fila 1

    Serial.begin(115200);
    Serial.println(F("LCD I2C lista."));
}

void loop() {
    lcd.setCursor(0, 1);
    lcd.print("TIEMPO: ");
    lcd.print(millis() / 1000);       // segundos desde que arranco
    lcd.print(" s   ");               // espacios borran digitos sobrantes

    delay(500);                       // aqui si es aceptable: solo muestra datos
}
