/*
 * 06_alerta_luz.ino - Integrador: sensor LDR + LED + buzzer
 * Monitoria: Introduccion al ESP32 - Intro. Ing. Electronica (UNAL)
 * Placa: ESP32-WROOM-32, ESP32-C6 o ESP32-S3
 * Concepto: sensado analogico, umbral con histeresis, actuadores, millis()
 * Montaje: LDR entre 3V3 y el pin analogico, resistencia de 10 kohm de ese
 *          pin a GND. LED en GPIO21 (220 ohm a GND). Buzzer pasivo en GPIO22.
 *
 *   WROOM-32 -> LDR en GPIO32
 *   ESP32-C6 -> LDR en GPIO0
 *   ESP32-S3 -> LDR en GPIO1
 */

// La placa seleccionada en el IDE define estas macros: no hay que tocar nada.
#if defined(HELTEC_V3)
const uint8_t PIN_LDR = 2;    // Heltec V3: GPIO2 (ADC1_CH1); GPIO1 es la bateria
#elif defined(ARDUINO_ESP32C6_DEV) || defined(CONFIG_IDF_TARGET_ESP32C6)
const uint8_t PIN_LDR = 0;    // ESP32-C6 DevKitC-1: GPIO0 (ADC1_CH0)
#elif defined(ARDUINO_ESP32S3_DEV) || defined(CONFIG_IDF_TARGET_ESP32S3)
const uint8_t PIN_LDR = 1;    // ESP32-S3 DevKitC-1: GPIO1 (ADC1_CH0)
#else
const uint8_t PIN_LDR = 32;   // ESP32-WROOM-32: GPIO32 (ADC1_CH4)
#endif

#if defined(HELTEC_V3)
const uint8_t LED    = 4;     // Heltec V3: GPIO21 es OLED_RST
const uint8_t BUZZER = 7;     // Heltec V3: GPIO22 no existe en el S3
#else
const uint8_t LED    = 21;
const uint8_t BUZZER = 22;
#endif

const int UMBRAL     = 2000;  // calibrar con el Monitor Serie
const int HISTERESIS = 150;   // evita el titileo cerca del umbral
const uint16_t INTERVALO_MS = 200;

bool alarma = false;
unsigned long tUltimo = 0;

void setup() {
    pinMode(LED, OUTPUT);
    pinMode(BUZZER, OUTPUT);
    analogReadResolution(12);
    Serial.begin(115200);
    delay(500);
    Serial.println(F("Tapa el LDR para activar la alarma."));
}

void loop() {
    if (millis() - tUltimo < INTERVALO_MS) {
        return;
    }
    tUltimo = millis();

    int luz = analogRead(PIN_LDR);   // oscuro -> valor bajo (LDR arriba)

    // Histeresis: enciende por debajo de UMBRAL-H, apaga por encima de UMBRAL+H
    if (!alarma && luz < (UMBRAL - HISTERESIS)) {
        alarma = true;
    } else if (alarma && luz > (UMBRAL + HISTERESIS)) {
        alarma = false;
    }

    digitalWrite(LED, alarma ? HIGH : LOW);

    if (alarma) {
        tone(BUZZER, 1200);          // buzzer pasivo: necesita frecuencia
    } else {
        noTone(BUZZER);
    }

    Serial.print(F("Luz="));
    Serial.print(luz);
    Serial.print(F("  Alarma="));
    Serial.println(alarma ? F("ON") : F("OFF"));
}
