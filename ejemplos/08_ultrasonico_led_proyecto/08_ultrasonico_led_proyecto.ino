/*
 * 08_ultrasonico_led_proyecto.ino - HC-SR04 con LED de aviso
 * Monitoria: Introduccion al ESP32 - Intro. Ing. Electronica (UNAL)
 * Placa: ESP32-WROOM-32 o ESP32-C6 DevKitC-1
 * Concepto: pulseIn(), sensor de tiempo, umbral de distancia, millis()
 * Montaje: HC-SR04 con VCC a 5V, GND a GND, TRIG en GPIO18 y ECHO en GPIO19
 *          a traves de un divisor (1 kohm entre ECHO y el pin, 2 kohm del pin
 *          a GND): el ECHO del sensor entrega 5 V y el ESP32 solo aguanta 3.3 V.
 *          LED en GPIO21 con resistencia de 220 ohm a GND.
 *
 * Si el objeto esta a menos de UMBRAL_CM, el LED se enciende. Es la base del
 * proyecto final (sensor de parqueadero).
 */

const uint8_t PIN_TRIG = 18;
const uint8_t PIN_ECHO = 19;
const uint8_t PIN_LED  = 21;

const float UMBRAL_CM = 20.0;        // distancia que enciende el LED
const uint16_t INTERVALO_MS = 100;   // el sensor necesita aire entre disparos
const uint32_t TIMEOUT_US = 30000;   // ~5 m: si no hay eco, pulseIn devuelve 0

unsigned long tUltimo = 0;

void setup() {
    pinMode(PIN_TRIG, OUTPUT);
    pinMode(PIN_ECHO, INPUT);
    pinMode(PIN_LED, OUTPUT);

    digitalWrite(PIN_TRIG, LOW);
    Serial.begin(115200);
    delay(500);
    Serial.println(F("Acerca la mano al sensor (umbral 20 cm)."));
}

void loop() {
    if (millis() - tUltimo < INTERVALO_MS) {
        return;                       // aun no toca medir
    }
    tUltimo = millis();

    // Pulso de 10 us en TRIG: el sensor responde con un pulso en ECHO
    digitalWrite(PIN_TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(PIN_TRIG, LOW);

    // El ancho del pulso en ECHO es el tiempo de ida y vuelta del sonido
    unsigned long duracion = pulseIn(PIN_ECHO, HIGH, TIMEOUT_US);

    if (duracion == 0) {              // sin eco: objeto muy lejos o mal cableado
        digitalWrite(PIN_LED, LOW);
        Serial.println(F("Sin eco"));
        return;
    }

    // 58 us por cm: sonido a 343 m/s, ida y vuelta
    float distancia = duracion / 58.0;

    digitalWrite(PIN_LED, distancia < UMBRAL_CM ? HIGH : LOW);

    Serial.print(F("Distancia: "));
    Serial.print(distancia, 1);
    Serial.print(F(" cm"));
    if (distancia < UMBRAL_CM) {
        Serial.print(F("  <- LED encendido"));
    }
    Serial.println();
}
