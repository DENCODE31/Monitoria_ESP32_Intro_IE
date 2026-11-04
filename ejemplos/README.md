# Ejemplos de código

Sketches autocontenidos: cada carpeta tiene un `.ino` con el mismo nombre y **funciona igual en WROOM-32 y en ESP32-C6**, salvo el pin del ADC que se resuelve con `#if defined(ARDUINO_ESP32C6_DEV) || defined(CONFIG_IDF_TARGET_ESP32C6)`.

| # | Carpeta | Concepto | Pines |
|---|---|---|---|
| 00 | `00_hola_serial` | Info del chip, `Serial` a 115200 | — |
| 01 | `01_blink` | `setup/loop`, salida digital | GPIO21 |
| 02 | `02_semaforo` | Secuencia con 3 LEDs | 21, 22, 23 |
| 03 | `03_pwm` | `analogWrite`, respiración | GPIO21 |
| 04 | `04_boton` | `INPUT_PULLUP`, antirrebote | 14, 21 |
| 05 | `05_potenciometro` | ADC 12 bits, `map`, Serial | 32 / 0, 21 |
| 06 | `06_alerta_luz` | LDR + LED + buzzer, histéresis | 32 / 0, 21, 22 |
| 07 | `07_wifi_scan` | `WiFi.scanNetworks()` | — |

## Cómo abrir un ejemplo

1. **File → Open** y selecciona el `.ino`.
2. Verifica **Tools → Board**:
   - WROOM-32 → `ESP32 Dev Module`
   - C6 → `ESP32C6 DevKitC-1`
3. Verifica **Tools → Port**.
4. **Upload (→)** y abre el Monitor Serie a **115200**.

## Con `arduino-cli`

```bash
arduino-cli core install esp32:esp32
arduino-cli compile --fqbn esp32:esp32:esp32 ejemplos/01_blink
arduino-cli upload  --fqbn esp32:esp32:esp32 -p /dev/ttyUSB0 ejemplos/01_blink

# ESP32-C6
arduino-cli compile --fqbn esp32:esp32:esp32c6 ejemplos/01_blink
arduino-cli upload  --fqbn esp32:esp32:esp32c6 -p /dev/ttyACM0 ejemplos/01_blink
```

## Orden sugerido

```
00 → 01 → 02 → 03 → 04 → 05 → 06 → 07
```

Los ejemplos 03, 04, 05 y 06 ya usan `millis()`: a partir de ahí no se bloquea el programa con `delay()`.
