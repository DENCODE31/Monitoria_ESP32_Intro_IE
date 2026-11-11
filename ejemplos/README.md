# Ejemplos de código

Sketch autocontenido por carpeta: el `.ino` es para **Arduino IDE** y el `main.c` de la misma carpeta es la versión para **ESP-IDF v5.x** (VS Code + extensión ESP-IDF). Ambos funcionan igual en WROOM-32, ESP32-C6 y ESP32-S3, salvo el pin del ADC que se resuelve con `#if` (`ARDUINO_ESP32C6_DEV` / `ARDUINO_ESP32S3_DEV` / `CONFIG_IDF_TARGET_ESP32C6` / `CONFIG_IDF_TARGET_ESP32S3`).

| # | Carpeta | Concepto | Pines |
|---|---|---|---|
| 00 | `00_hola_serial` | Info del chip, `Serial` a 115200 | — |
| 01 | `01_blink` | `setup/loop`, salida digital | GPIO21 |
| 02 | `02_semaforo` | Secuencia con 3 LEDs | 21, 22, 23 |
| 03 | `03_pwm` | `analogWrite`, respiración | GPIO21 |
| 04 | `04_boton` | `INPUT_PULLUP`, antirrebote | 14, 21 |
| 05 | `05_potenciometro` | ADC 12 bits, `map`, Serial | 32 / 0 / 1, 21 |
| 06 | `06_alerta_luz` | LDR + LED + buzzer, histéresis | 32 / 0 / 1, 21, 22 |
| 07 | `07_wifi_scan` | `WiFi.scanNetworks()` | — |
| 08 | `08_ultrasonico_led_proyecto` | `pulseIn()`, umbral de distancia, LED del proyecto final | 18, 19, 21 |
| 09 | `09_ldr_led_proyecto` | `analogRead`, umbral de luz, LED del proyecto final | 32 / 0 / 1, 21 |

## Versión ESP-IDF (`main.c`)

Cada carpeta trae un `main.c` con la misma lógica que el `.ino`, escrito con la API de ESP-IDF v5.x. No es un proyecto completo: es el archivo que se reemplaza dentro de un proyecto generado por la extensión.

1. En VS Code: **ESP-IDF: New Project** (o **Show Examples** → `get-started` → `sample_project`).
2. Elige el target: `esp32` (WROOM-32), `esp32c6` o `esp32s3`.
3. Reemplaza el contenido de `main/main.c` por el del ejemplo.
4. **ESP-IDF: Build** y **ESP-IDF: Flash**, luego abre el monitor a **115200**.

Equivalencias que cambian al pasar de Arduino a ESP-IDF:

| Arduino | ESP-IDF |
|---|---|
| `setup()` / `loop()` | `app_main()` con un `while (1)` |
| `delay(ms)` | `vTaskDelay(pdMS_TO_TICKS(ms))` |
| `millis()` | `esp_timer_get_time() / 1000` |
| `pinMode` / `digitalWrite` | `gpio_set_direction` / `gpio_set_level` |
| `analogWrite` | driver **LEDC** (temporizador + canal) |
| `analogRead` | driver **ADC oneshot** (unidad + canal) |
| `tone()` | LEDC con duty al 50 % |
| `Serial.print` | `printf` |
| `WiFi.scanNetworks()` | `esp_wifi_scan_start` + `esp_wifi_scan_get_ap_records` |

## Cómo abrir un ejemplo

1. **File → Open** y selecciona el `.ino`.
2. Verifica **Tools → Board**:
   - WROOM-32 → `ESP32 Dev Module`
   - C6 → `ESP32C6 DevKitC-1`
   - S3 → `ESP32S3 Dev Module`
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

# ESP32-S3
arduino-cli compile --fqbn esp32:esp32:esp32s3 ejemplos/01_blink
arduino-cli upload  --fqbn esp32:esp32:esp32s3 -p /dev/ttyACM0 ejemplos/01_blink
```

## Orden sugerido

```
00 → 01 → 02 → 03 → 04 → 05 → 06 → 07 → 08 → 09
```

Los ejemplos 03, 04, 05, 06, 08 y 09 ya usan `millis()`: a partir de ahí no se bloquea el programa con `delay()`.

El 08 y el 09 usan los pines del proyecto final (sensor de parqueadero), para no recablear después.
