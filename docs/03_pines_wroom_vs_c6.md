# 📌 Pines: WROOM-32 vs ESP32-C6 vs ESP32-S3

La monitoría usa **pines comunes** para que el mismo montaje y el mismo código funcionen en las tres placas. Solo la entrada analógica cambia.

---

## 1. Mapa de la monitoría

| Función | WROOM-32 | ESP32-C6 | ESP32-S3 | ¿Común? |
|---|---|---|---|---|
| LED 1 | GPIO21 | GPIO21 | GPIO21 | ✅ |
| LED 2 | GPIO22 | GPIO22 | GPIO22 | ✅ |
| LED 3 | GPIO23 | GPIO23 | GPIO23 | ✅ |
| Pulsador | GPIO14 | GPIO14 | GPIO14 | ✅ |
| HC-SR04 TRIG | GPIO18 | GPIO18 | GPIO18 | ✅ |
| HC-SR04 ECHO | GPIO19 | GPIO19 | GPIO19 ⚠️ | ⚠️ |
| Buzzer | GPIO22 | GPIO22 | GPIO22 | ✅ |
| Entrada analógica (potenciómetro / LDR) | **GPIO32** | **GPIO0** | **GPIO1** | ❌ |

### Por qué esos pines

- **21, 22, 23:** libres en las tres placas, sin strapping, sin función de flash, soportan PWM. En el WROOM son I²C/SPI por defecto, pero no se usan esos buses en la sesión.
- **14:** libre en las tres, sin strapping, admite `INPUT_PULLUP`.
- **GPIO32 (WROOM):** canal ADC1_CH4, sigue funcionando con Wi-Fi activo (a diferencia de ADC2).
- **GPIO0 (C6):** canal ADC1_CH0. En el C6 la lista de strapping es 8, 9 y 15, así que GPIO0 es seguro. **No** es el mismo caso que el GPIO0 del WROOM, que sí es strapping.
- **GPIO1 (S3):** canal ADC1_CH0, libre y sin strapping. El S3 tiene el ADC1 repartido en GPIO1–GPIO10.
- **GPIO19 (S3):** es `USB D-` del USB nativo. Sirve como ECHO mientras no se use ese USB; si el montaje falla, mueve ECHO a GPIO4.

---

## 2. Cómo se resuelve en el código

Los sketches de ADC llevan estas líneas al inicio:

```cpp
#if defined(ARDUINO_ESP32C6_DEV) || defined(CONFIG_IDF_TARGET_ESP32C6)
const uint8_t PIN_ADC = 0;    // ESP32-C6 DevKitC-1: GPIO0 (ADC1_CH0)
#elif defined(ARDUINO_ESP32S3_DEV) || defined(CONFIG_IDF_TARGET_ESP32S3)
const uint8_t PIN_ADC = 1;    // ESP32-S3 DevKitC-1: GPIO1 (ADC1_CH0)
#else
const uint8_t PIN_ADC = 32;   // ESP32-WROOM-32: GPIO32 (ADC1_CH4)
#endif
```

`ARDUINO_ESP32C6_DEV` y `ARDUINO_ESP32S3_DEV` las define la variante de placa del core; `CONFIG_IDF_TARGET_ESP32C6` y `CONFIG_IDF_TARGET_ESP32S3` vienen de la configuración de ESP-IDF. Se consultan las dos porque no siempre están ambas visibles desde el sketch: con una basta para elegir el pin correcto. No hay que tocar nada, solo seleccionar la placa en el IDE.

En ESP-IDF además del GPIO hay que dar el **canal** del ADC (`ADC_CHANNEL_4` en el WROOM, `ADC_CHANNEL_0` en C6 y S3).

---

## 3. Pines prohibidos

### ESP32-WROOM-32

```
GPIO 0   strapping (modo descarga)
GPIO 1   UART0 TX (USB)
GPIO 2   strapping + LED integrado
GPIO 3   UART0 RX (USB)
GPIO 4   libre (ADC2: no usar con Wi-Fi)
GPIO 5   strapping
GPIO 6-11  FLASH SPI — prohibido
GPIO 12  strapping MTDI — PELIGROSO
GPIO 13  libre (ADC2)
GPIO 14  libre ← usado para el botón
GPIO 15  strapping
GPIO 16,17  libres en WROOM (reservados en WROVER)
GPIO 18,19  libres (SPI)
GPIO 21,22,23  libres ← LEDs
GPIO 25,26  DAC / ADC2
GPIO 27  libre (ADC2)
GPIO 32,33  ADC1 ← potenciómetro
GPIO 34-39  SOLO ENTRADA, sin pull-up
```

### ESP32-C6

```
GPIO 0-7    libres (0 = ADC1_CH0) ← potenciómetro
GPIO 8      strapping + LED RGB integrado
GPIO 9      strapping (botón BOOT)
GPIO 10,11  libres
GPIO 12,13  USB D-/D+ (USB-JTAG)
GPIO 14     libre ← botón
GPIO 15     strapping
GPIO 16,17  UART0 por defecto
GPIO 18-23  libres ← LEDs en 21,22,23
GPIO 24-29  FLASH SPI — prohibido
```

---

## 4. Tabla rápida: ¿puedo usar este pin?

| GPIO | WROOM-32 | ESP32-C6 |
|---|---|---|
| 0 | ⚠️ strapping | ✅ ADC |
| 1 | ❌ UART0 | ❌ UART0 |
| 2 | ⚠️ strapping / LED | ✅ |
| 3 | ❌ UART0 | ✅ |
| 4 | ✅ (ADC2) | ✅ |
| 5 | ⚠️ strapping | ✅ |
| 6–11 | ❌ flash | ✅ (6,7,10,11) |
| 12 | ❌ **peligro** | ❌ USB |
| 13 | ✅ | ❌ USB |
| 14 | ✅ | ✅ |
| 15 | ⚠️ strapping | ⚠️ strapping |
| 16,17 | ✅ | ❌ UART0 |
| 18,19 | ✅ | ✅ |
| 21,22,23 | ✅ | ✅ |
| 24–29 | — | ❌ flash |
| 32,33 | ✅ ADC1 | — |
| 34–39 | ⚠️ solo entrada | — |

✅ usable · ⚠️ con cuidado · ❌ no usar

---

## 5. Alimentación

| Pin | WROOM-32 DevKit | C6 DevKitC-1 |
|---|---|---|
| `3V3` | Salida 3.3 V (~600 mA del regulador) | Salida 3.3 V |
| `5V` / `VIN` | Entrada 5 V del USB o fuente | Entrada 5 V |
| `GND` | Común | Común |

Regla: **el ESP32 alimenta sensores pequeños, no motores**. Un servo o un relé va con fuente externa y GND común.

---

## 6. Cómo identificar tu placa

1. Lee la serigrafía junto al módulo metálico: `ESP32-WROOM-32` o `ESP32-C6-WROOM-1`.
2. Cuenta los pines del módulo: el C6 es más pequeño y de una sola fila.
3. Conéctala y abre el Monitor Serie con `ejemplos/00_hola_serial`: imprime el modelo exacto del chip (`ESP32-C6` o `ESP32-D0WDQ6`).

> Si el IDE tiene seleccionada la placa equivocada, el programa compila pero no arranca o el Wi-Fi no funciona. Ese es el error de configuración más común de la sesión.

---

## 7. Heltec WiFi LoRa 32 V3 (ESP32-S3 con LoRa)

Esta placa **no** puede usar el mapa común: su S3 no tiene GPIO22 ni GPIO23, y muchos pines están ocupados por hardware interno.

| Pines ocupados | Función en la placa |
|---|---|
| GPIO8–14 | radio SX1262 (`NSS`, `SCK`, `MOSI`, `MISO`, `RST`, `BUSY`, `DIO1`) |
| GPIO17, 18 | I²C de la pantalla OLED (`SDA`, `SCL`) |
| GPIO21 | `OLED_RST` |
| GPIO35 | LED blanco integrado |
| GPIO36 | control de `Vext` (LOW = encendido) |
| GPIO37 | habilita el divisor de la batería |
| GPIO1 | lectura de la batería (390 k / 100 k) |
| GPIO0 | botón PRG |
| GPIO19, 20 | USB nativo (`D-`, `D+`) |
| GPIO26, 33–37 | flash / PSRAM: no usar |

### Mapa de la monitoría en el Heltec V3

| Función | Pin | ADC |
|---|---|---|
| LED 1 / 2 / 3 | GPIO4, GPIO5, GPIO6 | — |
| Buzzer | GPIO7 | — |
| Pulsador | GPIO20 | — |
| HC-SR04 TRIG / ECHO | GPIO47 / GPIO48 | — |
| LDR o potenciómetro | **GPIO2** | `ADC1_CH1` |
| LED integrado (opcional) | GPIO35 | — |

### Cómo activarlo

Los ejemplos traen bloques `#elif defined(HELTEC_V3)`. La macro no la define el core: hay que declararla.

- **Arduino IDE:** escribe `#define HELTEC_V3` en la primera línea del sketch, antes de todo.
- **ESP-IDF:** escribe `#define HELTEC_V3` justo después de los `#include`, o agrégalo en `main/CMakeLists.txt` con `target_compile_definitions(${COMPONENT_LIB} PRIVATE HELTEC_V3)`.
- **Placa en Arduino IDE:** `Heltec WiFi LoRa 32(V3)` o, con `arduino-cli`, `esp32:esp32:heltec_wifi_lora_32_V3`.

Sin esa macro los ejemplos usan el mapa de WROOM-32 / C6 / S3 y los LEDs o el sensor no responden en el Heltec.
