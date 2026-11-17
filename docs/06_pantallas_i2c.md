# 🖥️ Pantallas I2C: LCD 16x2 y OLED SSD1306

Guía para los ejemplos `12_lcd_i2c` y `13_oled_i2c`. Aplica igual en Arduino IDE y en ESP-IDF; cambian los pines del bus y las librerías.

---

## 1. Qué es I2C

Bus de dos hilos:

| Línea | Función |
|---|---|
| `SDA` | datos |
| `SCL` | reloj |

Cada dispositivo tiene una **dirección de 7 bits**. Varios dispositivos comparten el mismo par de cables; el maestro (la placa) habla con uno a la vez.

La dirección **no** la eliges por software: viene en el módulo.

| Dispositivo | Dirección habitual | Alternativa |
|---|---|---|
| LCD 16x2 + PCF8574 | `0x27` | `0x3F` |
| OLED SSD1306 128x64 | `0x3C` | `0x3D` |

---

## 2. Cableado

### LCD 16x2 con módulo I2C

| Módulo | ESP32 | UNO |
|---|---|---|
| VCC | `3V3` | `5V` |
| GND | `GND` | `GND` |
| SDA | según placa (ver tabla abajo) | `A4` |
| SCL | según placa | `A5` |

Ajusta el contraste con el potenciometro pequeño del módulo. Si la luz de fondo no enciende, revisa el jumper de backlight.

> Muchos módulos de LCD traen resistencias pull-up a 5 V. Alimentar el módulo con 5 V deja SDA/SCL por encima de los 3.3 V del ESP32. Alimenta el módulo desde `3V3` o usa un level shifter de 4 canales.

### OLED SSD1306

| OLED | ESP32 | UNO |
|---|---|---|
| VCC | `3V3` | `3V3` o `5V`, según regulador del módulo |
| GND | `GND` | `GND` |
| SDA | según placa | `A4` |
| SCL | según placa | `A5` |

El OLED consume poca corriente, pero es sensible al voltaje: revisa la etiqueta del regulador antes de conectarlo a 5 V.

---

## 3. Pines del bus en cada placa

| Placa | SDA | SCL | Cómo se activa |
|---|---|---|---|
| ESP32-WROOM-32 | 21 | 22 | Mapa por defecto de `Wire` |
| ESP32-C6 DevKitC-1 | 6 | 7 | Explícito: `Wire.begin(SDA, SCL)` |
| ESP32-S3 DevKit | 8 | 9 | Explícito: `Wire.begin(SDA, SCL)` |
| Heltec WiFi Kit 32 V3 | 17 | 18 | `#define HELTEC_V3` en el sketch |

En ESP-IDF los mismos números se ponen en `i2c_master_bus_config_t`.

> En la Heltec V3 el OLED interno ya ocupa `0x3C` en GPIO17/18. Si conectas otro OLED externo con la misma dirección, chocan. Cambia su dirección (algunos módulos tienen un jumper `0x3D`) o usa el OLED interno.

---

## 4. Librerías en Arduino IDE

Menú **Programa → Incluir librería → Administrar librerías**:

| Ejemplo | Librerías |
|---|---|
| LCD 16x2 | `LiquidCrystal_I2C` (John Rickman) |
| OLED | `Adafruit GFX Library` + `Adafruit SSD1306` |

Sin estas librerías el IDE marca error en el `#include`. No vienen con el core.

---

## 5. Escáner I2C

Si no sabes la dirección de tu módulo, sube este sketch y abre el Monitor Serie:

```cpp
#include <Wire.h>

void setup() {
    Serial.begin(115200);
    Wire.begin();                 // UNO: A4/A5
    Serial.println("Escaneando I2C...");
}

void loop() {
    int encontrados = 0;

    for (uint8_t direccion = 1; direccion < 127; direccion++) {
        Wire.beginTransmission(direccion);
        uint8_t respuesta = Wire.endTransmission();

        if (respuesta == 0) {
            Serial.print("Dispositivo en 0x");
            if (direccion < 16) Serial.print("0");
            Serial.println(direccion, HEX);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        Serial.println("No se encontro ningun dispositivo.");
    }

    delay(5000);
}
```

En ESP32, reemplaza `Wire.begin();` por `Wire.begin(SDA_PIN, SCL_PIN);` con los pines de tu placa.

---

## 6. ESP-IDF: detalle del `main.c`

Los `main.c` de los ejemplos 12 y 13 usan el driver nuevo de I2C:

```c
#include "driver/i2c_master.h"

i2c_master_bus_config_t config = {
    .i2c_port = I2C_NUM_0,
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .sda_io_num = PIN_SDA,
    .scl_io_num = PIN_SCL,
    .glitch_ignore_cnt = 7,
    .flags.enable_internal_pullup = true,
};
i2c_new_master_bus(&config, &bus);
```

Si aparece error de enlazado con `i2c_new_master_bus`, agrega el componente en `main/CMakeLists.txt`:

```cmake
idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_driver_i2c)
```

El ejemplo 12 escribe al PCF8574 bit a bit para generar los pulsos `E` del HD44780. El ejemplo 13 dibuja en un buffer de 8 x 128 bytes y lo envía página por página al SSD1306.

---

## 7. Errores frecuentes

| Síntoma | Causa | Solución |
|---|---|---|
| LCD enciende pero no muestra texto | Contraste mal ajustado | Girar el potenciometro del módulo |
| OLED no responde | Dirección distinta | Probar `0x3D` con el escáner |
| Escáner no encuentra nada | SDA/SCL invertidos o cable suelto | Revisar el cableado y los pines de la placa |
| `No se encontro el OLED` en serie | Mismo problema | Igual que la fila anterior |
| Basura en pantalla al iniciar | Falta secuencia de init | Usar los `main.c` de los ejemplos tal cual |
| ESP32 se reinicia al conectar el LCD | Pull-ups a 5 V | Alimentar módulo desde `3V3` o usar level shifter |
| Error de compilación por `#include` | Librerías no instaladas | Instalarlas desde el Administrador de librerías |
