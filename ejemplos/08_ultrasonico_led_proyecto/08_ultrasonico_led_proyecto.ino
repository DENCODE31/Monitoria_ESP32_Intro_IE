/*
 * 08_ultrasonico_led_proyecto.ino - HC-SR04 con LED de aviso
 * Monitoria: Introduccion al ESP32 - Intro. Ing. Electronica (UNAL)
 * Placa: ESP32-WROOM-32 o ESP32-C6 DevKitC-1
 * Concepto: pulseIn(), sensor de tiempo, umbral de distancia, millis()
 * Montaje: HC-SR04 con VCC a 5V, GND a GND, TRIG en GPIO18 y ECHO en GPIO19
 *          a traves de un divisor (1 kohm entre ECHO y el pin, 2 kohm del pin
 *          a GND): el ECHO del sensor entrega 5 V y el ESP32 solo aguanta 3.3 V.
 *          LED en GPIO21 con resistencia de 220 ohm a GND.
 */

// ---------- NUMEROS DE LOS PINES ----------
const uint8_t PIN_TRIG = 18;          // TRIG: por aqui el ESP32 manda el disparo
const uint8_t PIN_ECHO = 19;          // ECHO: por aqui responde el sensor
const uint8_t PIN_LED  = 21;          // LED: avisa que hay algo cerca

// ---------- VALORES QUE PODEMOS CAMBIAR ----------
const float UMBRAL_CM = 20.0;         // si la distancia es menor a 20 cm, prende el LED
const uint16_t INTERVALO_MS = 100;    // espera 100 ms entre una medida y la siguiente
const uint32_t TIMEOUT_US = 30000;    // si el eco no llega en 30 ms, se descarta la medida

// ---------- MEMORIA DEL PROGRAMA ----------
unsigned long tUltimo = 0;            // guarda el instante de la ultima medida

void setup() {
    pinMode(PIN_TRIG, OUTPUT);        // TRIG manda senal: es una salida
    pinMode(PIN_ECHO, INPUT);         // ECHO recibe senal: es una entrada
    pinMode(PIN_LED, OUTPUT);         // el LED se enciende y se apaga: es una salida

    digitalWrite(PIN_TRIG, LOW);      // deja TRIG en 0 para que no dispare solo
    Serial.begin(115200);             // abre el monitor serial a 115200 baudios
    delay(500);                       // pausa corta para que alcances a abrir el monitor
    Serial.println(F("Acerca la mano al sensor (umbral 20 cm)."));   // mensaje inicial
}

void loop() {
    // millis() cuenta los milisegundos desde que arranco la placa
    if (millis() - tUltimo < INTERVALO_MS) {
        return;                       // todavia no pasan 100 ms: sale y vuelve a entrar
    }
    tUltimo = millis();               // guarda este instante como el ultimo medido

    // 1. el ESP32 manda un pulso de 10 microsegundos por TRIG
    digitalWrite(PIN_TRIG, HIGH);     // sube TRIG
    delayMicroseconds(10);            // lo mantiene arriba 10 us
    digitalWrite(PIN_TRIG, LOW);      // lo baja: el disparo ya salio

    // 2. el sensor responde con un pulso en ECHO; su ancho es el tiempo del sonido
    unsigned long duracion = pulseIn(PIN_ECHO, HIGH, TIMEOUT_US);   // mide el pulso en us

    // 3. si pulseIn devuelve 0, no hubo eco
    if (duracion == 0) {
        digitalWrite(PIN_LED, LOW);   // apaga el LED
        Serial.println(F("Sin eco")); // avisa en el monitor serial
        return;                       // no hay distancia que calcular
    }

    // 4. pasa de tiempo a distancia: el sonido tarda 58 us en ir y volver 1 cm
    float distancia = duracion / 58.0;   // resultado en centimetros

    // 5. compara con el umbral: cerca enciende, lejos apaga
    digitalWrite(PIN_LED, distancia < UMBRAL_CM ? HIGH : LOW);   // ? : es un if corto

    // 6. muestra el resultado en el monitor serial
    Serial.print(F("Distancia: "));   // etiqueta
    Serial.print(distancia, 1);       // el valor con 1 decimal
    Serial.print(F(" cm"));           // unidad
    if (distancia < UMBRAL_CM) {
        Serial.print(F("  <- LED encendido"));   // aviso extra cuando esta cerca
    }
    Serial.println();                 // salto de linea para la proxima medida
}
