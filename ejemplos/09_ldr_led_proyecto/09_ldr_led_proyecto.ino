/*
 * 09_ldr_led_proyecto.ino - LDR que enciende un LED al taparla
 * Monitoria: Introduccion al ESP32 - Intro. Ing. Electronica (UNAL)
 * Placa: ESP32-WROOM-32, ESP32-C6 o ESP32-S3
 * Concepto: analogRead(), resolucion de 12 bits, umbral, millis()
 * Montaje: LDR entre 3V3 y el pin analogico, y resistencia de 10 kohm de ese
 *          pin a GND (divisor de voltaje). LED en GPIO21 con 220 ohm a GND.
 *
 *   WROOM-32 -> LDR en GPIO32 (ADC1_CH4)
 *   ESP32-C6 -> LDR en GPIO0  (ADC1_CH0)
 *   ESP32-S3 -> LDR en GPIO1  (ADC1_CH0)
 * El programa elige el pin solo, segun la placa que escogiste en el IDE.
 */

// ---------- NUMEROS DE LOS PINES ----------
// Estas macros las define la placa elegida: no hay que tocar nada.
#if defined(ARDUINO_ESP32C6_DEV) || defined(CONFIG_IDF_TARGET_ESP32C6)
const uint8_t PIN_LDR = 0;            // ESP32-C6: GPIO0 es entrada analogica
#elif defined(ARDUINO_ESP32S3_DEV) || defined(CONFIG_IDF_TARGET_ESP32S3)
const uint8_t PIN_LDR = 1;            // ESP32-S3: GPIO1 es entrada analogica
#else
const uint8_t PIN_LDR = 32;           // ESP32-WROOM-32: GPIO32 es entrada analogica
#endif
const uint8_t PIN_LED = 21;           // LED: se enciende cuando hay poca luz

// ---------- VALORES QUE PODEMOS CAMBIAR ----------
// Si la lectura baja de este valor, se prende el LED.
// Calibren: pongan el valor a mitad de camino entre la lectura con luz y tapada.
const int UMBRAL = 800;               // sobre 0..4095 (el equivalente a 200 de 0..1023)

// ---------- VALORES FIJOS ----------
const uint16_t INTERVALO_MS = 100;    // lee el sensor cada 100 ms

// ---------- MEMORIA DEL PROGRAMA ----------
unsigned long tUltimo = 0;            // guarda el instante de la ultima lectura

void setup() {
    Serial.begin(115200);             // abre el monitor serial a 115200 baudios
    analogReadResolution(12);         // lectura de 0 a 4095 (12 bits)
    pinMode(PIN_LED, OUTPUT);         // el LED se enciende y apaga: es una salida

    delay(500);                       // pausa corta para que alcances a abrir el monitor
    Serial.println(F("Tapa el LDR: la lectura baja y el LED se enciende."));
}

void loop() {
    // millis() cuenta los milisegundos desde que arranco la placa
    if (millis() - tUltimo < INTERVALO_MS) {
        return;                       // todavia no pasan 100 ms: sale y vuelve a entrar
    }
    tUltimo = millis();               // guarda este instante como el ultimo leido

    // 1. lee el pin analogico: con mucha luz el valor es alto, tapada es bajo
    int luz = analogRead(PIN_LDR);    // resultado entre 0 y 4095

    // 2. compara con el umbral para decidir si hay poca luz
    bool pocaLuz = (luz < UMBRAL);    // true = poca luz, false = mucha luz

    // 3. prende el LED solo cuando hay poca luz
    digitalWrite(PIN_LED, pocaLuz ? HIGH : LOW);   // ? : es un if corto

    // 4. muestra la lectura y el estado en el monitor serial
    Serial.print(F("luz (0-4095): "));   // etiqueta con el rango posible
    Serial.print(luz);                   // valor medido
    Serial.print(F("  LED="));           // separador
    Serial.println(pocaLuz ? F("ON") : F("OFF"));   // estado del LED
}
