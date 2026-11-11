/*
 * 03_pwm.ino - Intensidad variable con PWM (efecto respiracion)
 * Monitoria: Introduccion al ESP32 - Intro. Ing. Electronica (UNAL)
 * Placa: ESP32-WROOM-32, ESP32-C6 o ESP32-S3
 * Concepto: analogWrite() 0-255, LEDC por debajo, cambio de direccion con millis()
 * Montaje: LED en GPIO21 con resistencia de 220 ohm a GND
 */

const uint8_t LED = 21;
const uint8_t PASO = 3;
const uint16_t INTERVALO_MS = 15;

int brillo = 0;
int direccion = 1;
unsigned long tUltimo = 0;

void setup() {
    pinMode(LED, OUTPUT);
}

void loop() {
    if (millis() - tUltimo < INTERVALO_MS) {
        return;                       // aun no toca actualizar
    }
    tUltimo = millis();

    brillo += direccion * PASO;

    if (brillo >= 255) {              // techo: empieza a bajar
        brillo = 255;
        direccion = -1;
    } else if (brillo <= 0) {         // piso: empieza a subir
        brillo = 0;
        direccion = 1;
    }

    analogWrite(LED, brillo);         // duty cycle 0..255
}
