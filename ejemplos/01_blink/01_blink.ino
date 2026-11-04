/*
 * 01_blink.ino - Parpadeo de un LED externo
 * Monitoria: Introduccion al ESP32 - Intro. Ing. Electronica (UNAL)
 * Placa: ESP32-WROOM-32 o ESP32-C6 DevKitC-1
 * Concepto: setup()/loop(), pinMode(), digitalWrite(), delay()
 * Montaje: LED en GPIO21 con resistencia de 220 ohm a GND
 *
 * Nota: no usamos el LED integrado porque no es igual en ambas placas.
 *   WROOM-32 DevKit V1  -> LED simple en GPIO2
 *   ESP32-C6 DevKitC-1  -> LED RGB WS2812 en GPIO8 (necesita driver RGB)
 * Con un LED externo en GPIO21 el mismo montaje y codigo sirven en las dos.
 */

const uint8_t LED_PIN = 21;
const uint16_t PERIODO_MS = 500;

void setup() {
    pinMode(LED_PIN, OUTPUT);      // GPIO21 como salida
}

void loop() {
    digitalWrite(LED_PIN, HIGH);   // 3.3 V -> LED encendido
    delay(PERIODO_MS);
    digitalWrite(LED_PIN, LOW);    // 0 V -> LED apagado
    delay(PERIODO_MS);
}
