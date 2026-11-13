/*
 * 11_servo_buzzer.ino - Barrera con servo y aviso sonoro
 * Monitoria: Introduccion al ESP32 - Intro. Ing. Electronica (UNAL)
 * Placa: ESP32-WROOM-32, ESP32-C6 o ESP32-S3
 * Concepto: integrar sensor, servo y buzzer en un solo programa no bloqueante
 * Montaje: HC-SR04 (VCC=5V, TRIG=GPIO18, ECHO=GPIO19 por divisor, GND=GND)
 *          Servo: senal en GPIO21, VCC externo de 5 V, GND comun
 *          Buzzer pasivo en GPIO22
 *
 * Heltec WiFi Kit / LoRa 32 V3: TRIG 47, ECHO 48, servo 5, buzzer 7.
 *
 * OJO: el servo NO se alimenta del 3V3. Consume picos de 500 mA.
 *      Buzzer pasivo: necesita frecuencia; uno activo solo necesita HIGH.
 *
 * Aviso sonoro: un pitido al abrir y otro distinto al cerrar. El pitido se
 * apaga con millis(), no con delay(), para no frenar la medicion.
 */

// ---------- NUMEROS DE LOS PINES ----------
#if defined(HELTEC_V3)
const uint8_t PIN_TRIG   = 47;        // Heltec V3: GPIO18 es OLED SCL
const uint8_t PIN_ECHO   = 48;        // Heltec V3: GPIO19 es USB D-
const uint8_t PIN_SERVO  = 5;         // Heltec V3: GPIO21 es OLED_RST
const uint8_t PIN_BUZZER = 7;         // Heltec V3: GPIO22 no existe en el S3
#else
const uint8_t PIN_TRIG   = 18;        // TRIG: el ESP32 manda el disparo
const uint8_t PIN_ECHO   = 19;        // ECHO: el sensor responde
const uint8_t PIN_SERVO  = 21;        // senal del servo (PWM de 50 Hz)
const uint8_t PIN_BUZZER = 22;        // buzzer pasivo
#endif

// ---------- VALORES QUE PODEMOS CAMBIAR ----------
const float CERCA_CM = 15.0;          // si algo esta a menos de 15 cm, abre
const float LEJOS_CM = 20.0;          // si se aleja mas de 20 cm, cierra (histeresis)
const uint16_t INTERVALO_MS = 100;    // mide cada 100 ms
const uint32_t TIMEOUT_US = 30000;    // si no hay eco en 30 ms, se descarta

// ---------- ANGULOS DE LA BARRA ----------
const int ANGULO_CERRADO = 0;         // barra horizontal: paso bloqueado
const int ANGULO_ABIERTO = 90;        // barra levantada: paso libre

// ---------- SONIDOS ----------
const uint16_t FREQ_ABRE   = 1500;    // Hz al abrir (agudo)
const uint16_t FREQ_CIERRA = 800;     // Hz al cerrar (grave)
const uint16_t DURACION_MS = 150;     // cuanto dura cada pitido

// ---------- MEMORIA DEL PROGRAMA ----------
bool abierta = false;                 // estado actual de la barrera
unsigned long tUltimo = 0;            // instante de la ultima medida
bool pitando = false;                 // hay un pitido en curso
unsigned long tPitido = 0;            // instante en que empezo el pitido

// Mueve el servo a un angulo entre 0 y 180 grados
void mover_servo(int grados) {
    int us = map(grados, 0, 180, 500, 2500);          // angulo a microsegundos
    uint32_t duty = (uint32_t)us * 65536UL / 20000UL; // us a duty de 16 bits
    ledcWrite(PIN_SERVO, duty);                       // aplica el pulso
}

// Arranca un pitido sin bloquear el programa
void pitido(uint16_t frecuencia) {
    tone(PIN_BUZZER, frecuencia);     // genera la onda cuadrada
    tPitido = millis();               // guarda cuando empezo
    pitando = true;                   // marca que hay un pitido activo
}

void setup() {
    ledcAttach(PIN_SERVO, 50, 16);    // GPIO del servo, 50 Hz, 16 bits
    pinMode(PIN_TRIG, OUTPUT);        // TRIG manda senal: salida
    pinMode(PIN_ECHO, INPUT);         // ECHO recibe senal: entrada
    pinMode(PIN_BUZZER, OUTPUT);      // buzzer: salida
    digitalWrite(PIN_TRIG, LOW);      // TRIG empieza en 0

    mover_servo(ANGULO_CERRADO);      // arranca con la barra cerrada

    Serial.begin(115200);             // abre el monitor serial
    delay(500);
    Serial.println(F("Barrera con aviso sonoro lista."));
}

void loop() {
    // 1. apaga el pitido cuando ya paso su duracion (no bloqueante)
    if (pitando && millis() - tPitido >= DURACION_MS) {
        noTone(PIN_BUZZER);           // silencia el buzzer
        pitando = false;              // ya no hay pitido activo
    }

    // 2. mide cada 100 ms
    if (millis() - tUltimo < INTERVALO_MS) {
        return;
    }
    tUltimo = millis();

    // 3. disparo de 10 us por TRIG y lectura del eco
    digitalWrite(PIN_TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(PIN_TRIG, LOW);

    unsigned long duracion = pulseIn(PIN_ECHO, HIGH, TIMEOUT_US);
    if (duracion == 0) {                       // sin eco
        Serial.println(F("Sin eco"));
        return;
    }

    float distancia = duracion / 58.0;         // 58 us por cm

    // 4. decide con histeresis y avisa con un sonido distinto cada caso
    if (!abierta && distancia < CERCA_CM) {
        abierta = true;
        mover_servo(ANGULO_ABIERTO);           // levanta la barra
        pitido(FREQ_ABRE);                    // pitido agudo al abrir
    } else if (abierta && distancia > LEJOS_CM) {
        abierta = false;
        mover_servo(ANGULO_CERRADO);           // baja la barra
        pitido(FREQ_CIERRA);                  // pitido grave al cerrar
    }

    // 5. muestra el estado en el monitor serial
    Serial.print(F("Distancia: "));
    Serial.print(distancia, 1);
    Serial.print(F(" cm  barrera="));
    Serial.println(abierta ? F("ABIERTA") : F("CERRADA"));
}
