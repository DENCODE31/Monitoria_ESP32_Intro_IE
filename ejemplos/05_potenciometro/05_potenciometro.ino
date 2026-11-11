/*
 * 05_potenciometro.ino - Lectura analogica de 12 bits
 * Monitoria: Introduccion al ESP32 - Intro. Ing. Electronica (UNAL)
 * Placa: ESP32-WROOM-32, ESP32-C6 o ESP32-S3
 * Concepto: analogRead(), resolucion 12 bits, map(), Monitor Serie
 * Montaje: potenciometro (extremos a 3V3 y GND, cursor al pin analogico)
 *          LED en GPIO21 con 220 ohm a GND
 *
 * El unico pin que cambia entre placas es el del ADC:
 *   WROOM-32 -> GPIO32 (ADC1_CH4)
 *   ESP32-C6 -> GPIO0  (ADC1_CH0)
 *   ESP32-S3 -> GPIO1  (ADC1_CH0)
 */

// La placa seleccionada en el IDE define estas macros: no hay que tocar nada.
// ARDUINO_ESP32C6_DEV la pone la variante C6; CONFIG_IDF_TARGET_ESP32C6 viene del core.
#if defined(HELTEC_V3)
const uint8_t PIN_ADC = 2;    // Heltec V3: GPIO2 (ADC1_CH1); GPIO1 es la bateria
#elif defined(ARDUINO_ESP32C6_DEV) || defined(CONFIG_IDF_TARGET_ESP32C6)
const uint8_t PIN_ADC = 0;    // ESP32-C6 DevKitC-1: GPIO0 (ADC1_CH0)
#elif defined(ARDUINO_ESP32S3_DEV) || defined(CONFIG_IDF_TARGET_ESP32S3)
const uint8_t PIN_ADC = 1;    // ESP32-S3 DevKitC-1: GPIO1 (ADC1_CH0)
#else
const uint8_t PIN_ADC = 32;   // ESP32-WROOM-32: GPIO32 (ADC1_CH4)
#endif

#if defined(HELTEC_V3)
const uint8_t LED = 4;        // Heltec V3: GPIO21 es OLED_RST
#else
const uint8_t LED = 21;
#endif
const uint16_t INTERVALO_MS = 250;
const int UMBRAL = 2048;      // mitad de 4095

unsigned long tUltimo = 0;

void setup() {
    pinMode(LED, OUTPUT);
    analogReadResolution(12);         // 0..4095 (por defecto ya es 12)
    Serial.begin(115200);
    delay(500);
    Serial.println(F("Gira el potenciometro (0-4095)."));
}

void loop() {
    if (millis() - tUltimo < INTERVALO_MS) {
        return;
    }
    tUltimo = millis();

    int crudo = analogRead(PIN_ADC);              // 0..4095
    float voltaje = crudo * (3.3 / 4095.0);       // referencia de 3.3 V
    int brillo = map(crudo, 0, 4095, 0, 255);     // a rango PWM

    analogWrite(LED, brillo);

    Serial.print(F("ADC="));
    Serial.print(crudo);
    Serial.print(F("  V="));
    Serial.print(voltaje, 2);
    Serial.print(F(" V  PWM="));
    Serial.print(brillo);
    Serial.print(F("  LED="));
    Serial.println(crudo >= UMBRAL ? F("ON") : F("OFF"));
}
