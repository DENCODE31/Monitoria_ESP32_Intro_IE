/*
 * 10_servo_barrera.ino - HC-SR04 + servo: barrera que abre al acercarse
 * Monitoria: Introduccion al ESP32 - Intro. Ing. Electronica (UNAL)
 * Placa: ESP32-WROOM-32, ESP32-C6 o ESP32-S3
 * Concepto: combinar un sensor de distancia con un actuador (servo), PWM de
 *           50 Hz, histeresis para que la barra no tiemble
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

// ---------- NUMEROS DE LOS PINES ----------
#if defined(HELTEC_V3)
const uint8_t PIN_TRIG  = 47;         // Heltec V3: GPIO18 es OLED SCL
const uint8_t PIN_ECHO  = 48;         // Heltec V3: GPIO19 es USB D-
const uint8_t PIN_SERVO = 5;          // Heltec V3: GPIO21 es OLED_RST
#else
const uint8_t PIN_TRIG  = 18;         // TRIG: el ESP32 manda el disparo
const uint8_t PIN_ECHO  = 19;         // ECHO: el sensor responde
const uint8_t PIN_SERVO = 21;         // senal del servo (PWM de 50 Hz)
#endif

// ---------- VALORES QUE PODEMOS CAMBIAR ----------
const float CERCA_CM = 15.0;          // si algo esta a menos de 15 cm, abre
const float LEJOS_CM = 20.0;          // si se aleja mas de 20 cm, cierra (histeresis)
const uint16_t INTERVALO_MS = 100;    // mide cada 100 ms
const uint32_t TIMEOUT_US = 30000;    // si no hay eco en 30 ms, se descarta

// ---------- ANGULOS DE LA BARRA ----------
const int ANGULO_CERRADO = 0;         // barra horizontal: paso bloqueado
const int ANGULO_ABIERTO = 90;        // barra levantada: paso libre

// ---------- MEMORIA DEL PROGRAMA ----------
bool abierta = false;                 // estado actual de la barrera
unsigned long tUltimo = 0;            // instante de la ultima medida

// Mueve el servo a un angulo entre 0 y 180 grados
void mover_servo(int grados) {
    int us = map(grados, 0, 180, 500, 2500);          // angulo a microsegundos
    uint32_t duty = (uint32_t)us * 65536UL / 20000UL; // us a duty de 16 bits
    ledcWrite(PIN_SERVO, duty);                       // aplica el pulso
    // Core 2.x: ledcSetup(0, 50, 16); ledcAttachPin(PIN_SERVO, 0); ledcWrite(0, duty);
}

void setup() {
    ledcAttach(PIN_SERVO, 50, 16);    // GPIO del servo, 50 Hz, resolucion de 16 bits
    pinMode(PIN_TRIG, OUTPUT);        // TRIG manda senal: salida
    pinMode(PIN_ECHO, INPUT);         // ECHO recibe senal: entrada
    digitalWrite(PIN_TRIG, LOW);      // TRIG empieza en 0

    mover_servo(ANGULO_CERRADO);      // arranca con la barra cerrada

    Serial.begin(115200);             // abre el monitor serial
    delay(500);
    Serial.println(F("Barrera lista. Acerca la mano al sensor."));
}

void loop() {
    if (millis() - tUltimo < INTERVALO_MS) {   // todavia no pasan 100 ms...
        return;                                // sale y vuelve a entrar
    }
    tUltimo = millis();                        // guarda este instante

    // 1. disparo de 10 us por TRIG
    digitalWrite(PIN_TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(PIN_TRIG, LOW);

    // 2. el ancho del pulso en ECHO es el tiempo de ida y vuelta
    unsigned long duracion = pulseIn(PIN_ECHO, HIGH, TIMEOUT_US);
    if (duracion == 0) {                       // sin eco: no hay nada enfrente
        Serial.println(F("Sin eco"));
        return;
    }

    float distancia = duracion / 58.0;         // 58 us por cm (ida y vuelta)

    // 3. decide con histeresis: abre cerca, cierra solo si se aleja de verdad
    if (!abierta && distancia < CERCA_CM) {
        abierta = true;
        mover_servo(ANGULO_ABIERTO);           // levanta la barra
    } else if (abierta && distancia > LEJOS_CM) {
        abierta = false;
        mover_servo(ANGULO_CERRADO);           // baja la barra
    }

    // 4. muestra el estado en el monitor serial
    Serial.print(F("Distancia: "));
    Serial.print(distancia, 1);
    Serial.print(F(" cm  barrera="));
    Serial.println(abierta ? F("ABIERTA") : F("CERRADA"));
}
