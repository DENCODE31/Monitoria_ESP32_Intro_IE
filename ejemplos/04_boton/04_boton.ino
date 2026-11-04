/*
 * 04_boton.ino - Pulsador con antirrebote por software
 * Monitoria: Introduccion al ESP32 - Intro. Ing. Electronica (UNAL)
 * Placa: ESP32-WROOM-32 o ESP32-C6 DevKitC-1
 * Concepto: entrada digital, INPUT_PULLUP, rebote, contador con millis()
 * Montaje: pulsador entre GPIO14 y GND, LED en GPIO21 con 220 ohm a GND
 *
 * Ojo: con INPUT_PULLUP la logica se invierte -> LOW significa presionado.
 */

const uint8_t BOTON = 14;
const uint8_t LED   = 21;
const uint16_t DEBOUNCE_MS = 30;

bool lecturaAnterior = HIGH;
unsigned long tUltimoCambio = 0;
uint16_t contador = 0;

void setup() {
    pinMode(BOTON, INPUT_PULLUP);
    pinMode(LED, OUTPUT);
    Serial.begin(115200);
    delay(500);
    Serial.println(F("Presiona el boton (GPIO14)."));
}

void loop() {
    bool lectura = digitalRead(BOTON);

    // Solo acepta el cambio si paso el tiempo de antirrebote
    if (lectura != lecturaAnterior && (millis() - tUltimoCambio) > DEBOUNCE_MS) {
        tUltimoCambio = millis();
        lecturaAnterior = lectura;

        if (lectura == LOW) {              // flanco de bajada = pulsacion
            contador++;
            digitalWrite(LED, HIGH);
            Serial.print(F("Pulsaciones: "));
            Serial.println(contador);
        } else {                           // flanco de subida = liberacion
            digitalWrite(LED, LOW);
        }
    }
}
