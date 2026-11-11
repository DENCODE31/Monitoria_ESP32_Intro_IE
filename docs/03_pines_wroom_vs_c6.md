# 📌 Pines: WROOM-32 vs ESP32-C6

La monitoría usa **pines comunes** para que el mismo montaje y el mismo código funcionen en ambas placas. Solo la entrada analógica cambia.

---

## 1. Mapa de la monitoría

| Función | WROOM-32 | ESP32-C6 | ¿Común? |
|---|---|---|---|
| LED 1 | GPIO21 | GPIO21 | ✅ |
| LED 2 | GPIO22 | GPIO22 | ✅ |
| LED 3 | GPIO23 | GPIO23 | ✅ |
| Pulsador | GPIO14 | GPIO14 | ✅ |
| HC-SR04 TRIG | GPIO18 | GPIO18 | ✅ |
| HC-SR04 ECHO | GPIO19 | GPIO19 | ✅ |
| Buzzer | GPIO22 | GPIO22 | ✅ |
| Entrada analógica (potenciómetro / LDR) | **GPIO32** | **GPIO0** | ❌ |

### Por qué esos pines

- **21, 22, 23:** libres en ambas placas, sin strapping, sin función de flash, soportan PWM. En el WROOM son I²C/SPI por defecto, pero no se usan esos buses en la sesión.
- **14:** libre en ambas, sin strapping, admite `INPUT_PULLUP`.
- **GPIO32 (WROOM):** canal ADC1_CH4, sigue funcionando con Wi-Fi activo (a diferencia de ADC2).
- **GPIO0 (C6):** canal ADC1_CH0. En el C6 la lista de strapping es 8, 9 y 15, así que GPIO0 es seguro. **No** es el mismo caso que el GPIO0 del WROOM, que sí es strapping.

---

## 2. Cómo se resuelve en el código

Los sketches de ADC llevan dos líneas al inicio:

```cpp
#if defined(ARDUINO_ESP32C6_DEV) || defined(CONFIG_IDF_TARGET_ESP32C6)
const uint8_t PIN_ADC = 0;    // ESP32-C6 DevKitC-1: GPIO0 (ADC1_CH0)
#else
const uint8_t PIN_ADC = 32;   // ESP32-WROOM-32: GPIO32 (ADC1_CH4)
#endif
```

`ARDUINO_ESP32C6_DEV` la define la variante de placa del core y `CONFIG_IDF_TARGET_ESP32C6` viene de la configuración de ESP-IDF. Se consultan las dos porque no siempre están ambas visibles desde el sketch: con una basta para elegir el pin correcto. No hay que tocar nada, solo seleccionar la placa en el IDE.

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
