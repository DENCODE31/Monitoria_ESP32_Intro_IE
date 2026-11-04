/*
 * 07_wifi_scan.ino - Escaneo de redes Wi-Fi
 * Monitoria: Introduccion al ESP32 - Intro. Ing. Electronica (UNAL)
 * Placa: ESP32-WROOM-32 (Wi-Fi 4) o ESP32-C6 (Wi-Fi 6)
 * Concepto: que el ESP32 tiene radio integrada; primer contacto con WiFi.h
 * Montaje: ninguno
 *
 * Este ejemplo se copia del repositorio, no se escribe en clase: son
 * llamadas de configuracion que todavia no se han explicado.
 */

#include <WiFi.h>

const uint16_t INTERVALO_MS = 15000;   // escanea cada 15 s

unsigned long tUltimo = 0;

void setup() {
    Serial.begin(115200);
    delay(500);

    WiFi.mode(WIFI_STA);               // modo estacion (cliente)
    WiFi.disconnect();                 // sin conexion: solo escuchar
    delay(100);

    Serial.println(F("Escaneo Wi-Fi iniciado."));
}

void loop() {
    if (millis() - tUltimo < INTERVALO_MS) {
        return;
    }
    tUltimo = millis();

    Serial.println(F("Escaneando..."));
    int redes = WiFi.scanNetworks();   // bloquea unos segundos

    if (redes <= 0) {
        Serial.println(F("No se encontraron redes."));
        return;
    }

    Serial.print(redes);
    Serial.println(F(" redes encontradas:"));
    Serial.println(F("  #  SSID                             RSSI  Canal  Cifrado"));

    for (int i = 0; i < redes; i++) {
        Serial.printf("  %d  %-32s %4d  %5d  %s\n",
                      i + 1,
                      WiFi.SSID(i).c_str(),
                      WiFi.RSSI(i),
                      WiFi.channel(i),
                      WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "abierta" : "protegida");
    }

    WiFi.scanDelete();                 // libera la memoria del escaneo
    Serial.println();
}
