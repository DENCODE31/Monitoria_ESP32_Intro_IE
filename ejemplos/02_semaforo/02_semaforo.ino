/*
 * 02_semaforo.ino - Semaforo de tres estados
 * Monitoria: Introduccion al ESP32 - Intro. Ing. Electronica (UNAL)
 * Placa: ESP32-WROOM-32, ESP32-C6 o ESP32-S3
 * Concepto: varias salidas digitales, secuencia temporizada, funciones propias
 * Montaje: LED rojo en GPIO21, amarillo en GPIO22, verde en GPIO23 (220 ohm c/u)
 */

#if defined(HELTEC_V3)
// Heltec V3 usa ESP32-S3: no existen GPIO22 ni GPIO23.
const uint8_t LED_ROJO      = 4;
const uint8_t LED_AMARILLO  = 5;
const uint8_t LED_VERDE     = 6;
#else
const uint8_t LED_ROJO      = 21;
const uint8_t LED_AMARILLO  = 22;
const uint8_t LED_VERDE     = 23;
#endif

const uint16_t T_VERDE    = 3000;
const uint16_t T_AMARILLO = 1000;
const uint16_t T_ROJO     = 3000;

void apagarTodos() {
    digitalWrite(LED_ROJO, LOW);
    digitalWrite(LED_AMARILLO, LOW);
    digitalWrite(LED_VERDE, LOW);
}

void setup() {
    pinMode(LED_ROJO, OUTPUT);
    pinMode(LED_AMARILLO, OUTPUT);
    pinMode(LED_VERDE, OUTPUT);
    apagarTodos();
}

void loop() {
    digitalWrite(LED_VERDE, HIGH);
    delay(T_VERDE);
    digitalWrite(LED_VERDE, LOW);

    digitalWrite(LED_AMARILLO, HIGH);
    delay(T_AMARILLO);
    digitalWrite(LED_AMARILLO, LOW);

    digitalWrite(LED_ROJO, HIGH);
    delay(T_ROJO);
    digitalWrite(LED_ROJO, LOW);
}
