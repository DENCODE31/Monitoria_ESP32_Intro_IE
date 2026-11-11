/*
 * 00_hola_serial.ino - Conocer el chip por el Monitor Serie
 * Monitoria: Introduccion al ESP32 - Intro. Ing. Electronica (UNAL)
 * Placa: ESP32-WROOM-32, ESP32-C6 o ESP32-S3 (el codigo sirve en ambas)
 * Concepto: Serial.begin, impresion de datos, funciones ESP.get*()
 * Montaje: ninguno, solo el cable USB
 */

void setup() {
    Serial.begin(115200);       // 115200 es el baudaje habitual en ESP32
    delay(1500);                // da tiempo a abrir el Monitor Serie

    Serial.println();
    Serial.println(F("========================================"));
    Serial.println(F("  Monitoria: Introduccion al ESP32"));
    Serial.println(F("========================================"));

    Serial.print(F("Modelo del chip : "));
    Serial.println(ESP.getChipModel());

    Serial.print(F("Revision        : "));
    Serial.println(ESP.getChipRevision());

    Serial.print(F("Nucleos         : "));
    Serial.println(ESP.getChipCores());

    Serial.print(F("Frecuencia      : "));
    Serial.print(ESP.getCpuFreqMHz());
    Serial.println(F(" MHz"));

    Serial.print(F("Heap libre      : "));
    Serial.print(ESP.getFreeHeap());
    Serial.println(F(" bytes"));

    Serial.print(F("Version del SDK : "));
    Serial.println(ESP.getSdkVersion());

    Serial.println(F("----------------------------------------"));
    Serial.println(F("Si ves esto, el entorno esta listo."));
    Serial.println();
}

void loop() {
    // Mensaje periodico para comprobar que el programa sigue vivo
    static unsigned long tUltimo = 0;

    if (millis() - tUltimo >= 2000) {
        tUltimo = millis();
        Serial.print(F("Activo hace "));
        Serial.print(millis() / 1000);
        Serial.println(F(" s"));
    }
}
