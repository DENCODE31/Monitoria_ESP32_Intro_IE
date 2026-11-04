# ⚡ ESP32 vs Arduino UNO

Entender esta tabla evita el 90 % de los daños y de las confusiones de la sesión.

---

## 1. Comparación directa

| Característica | Arduino UNO R3 | ESP32-WROOM-32 | ESP32-C6 |
|---|---|---|---|
| CPU | ATmega328P, 8 bits, 16 MHz | Xtensa LX6, **dual core**, 240 MHz | RISC-V, **single core**, 160 MHz |
| Arquitectura | AVR | Xtensa | RISC-V |
| SRAM | 2 KB | 520 KB | 512 KB |
| Flash | 32 KB | 4 MB (externa) | 8 MB (externa) |
| Lógica | **5 V** | **3.3 V** | **3.3 V** |
| Tolerante a 5 V en GPIO | Sí (es su nivel) | **NO** | **NO** |
| ADC | 10 bits (0–1023) | 12 bits (0–4095) | 12 bits (0–4095) |
| DAC | No | Sí (GPIO25/26) | **No** |
| Wi-Fi | No | 802.11 b/g/n | **802.11ax (Wi-Fi 6)** |
| Bluetooth | No | Classic + BLE | BLE 5.3 |
| Zigbee/Thread | No | No | **Sí** |
| Corriente por pin | 20 mA | 20 mA recomendado (40 máx) | 20 mA recomendado (40 máx) |
| Pines GPIO | 20 | 34 (no todos expuestos) | 30 |
| USB | ATmega16U2 / CH340 | CH340 / CP2102 | **USB-JTAG nativo** |

---

## 2. Lo que cambia en la práctica

### 2.1 El voltaje: 3.3 V, no 5 V

```
Arduino UNO:  HIGH = 5 V     → un sensor de 5 V se conecta directo
ESP32:        HIGH = 3.3 V   → 5 V en un GPIO = chip muerto
```

Consecuencias:

- Un LED que en el UNO usaba 220 Ω, en el ESP32 funciona igual (la resistencia se calcula con 3.3 V, da un poco más de corriente; sigue sirviendo 220 Ω).
- **Un sensor de 5 V no se conecta directo**: hay que usar divisor de tensión o level shifter.
- El pin `VIN`/`5V` del DevKit es **entrada** (o salida del USB). No es una fuente de 5 V potente.
- Alimenta módulos de 5 V (relés, servos) con fuente externa y **GND común**.

### 2.2 El ADC es de 12 bits

```cpp
analogRead(pin);   // 0 .. 4095  (no 0 .. 1023)
```

El rango completo corresponde a ~3.3 V, no a 5 V. Si el código del UNO usaba `map(x, 0, 1023, 0, 255)`, en el ESP32 es `map(x, 0, 4095, 0, 255)`.

### 2.3 Casi cualquier pin sirve para casi todo

El **GPIO Matrix** permite enrutar PWM, I²C, SPI y UART a cualquier GPIO. Aun así, la sesión usa pines fijos para que el cableado sea el mismo en las dos placas.

### 2.4 PWM

`analogWrite()` funciona en el core 3.x (implementado con el periférico **LEDC**, 8 bits por defecto). También existe la API avanzada `ledcSetup()` / `ledcAttach()` si se necesita más resolución o frecuencia.

### 2.5 El LED integrado no siempre es un LED simple

| Placa | LED integrado | Implicación |
|---|---|---|
| WROOM-32 DevKit V1 | LED en **GPIO2** | `digitalWrite(2, HIGH)` funciona |
| ESP32-C6 DevKitC-1 | **LED RGB WS2812 en GPIO8** | Requiere driver RGB (`rgbLedWrite`), no `digitalWrite` |

Por eso los ejemplos usan un **LED externo en GPIO21**: el mismo código y el mismo cableado sirven en ambas placas.

---

## 3. Pines que NO se tocan

### ESP32-WROOM-32

| GPIO | Problema |
|---|---|
| **6 – 11** | Conectados a la flash SPI: usarlos congela el chip |
| **34, 35, 36, 39** | Solo entrada, sin pull-up interno |
| **0, 2, 5, 12, 15** | Strapping: definen el modo de arranque |
| **12** | **PELIGRO**: selecciona el voltaje de flash (1.8 V / 3.3 V). Puede dejar la placa inservible |
| 1, 3 | UART0 (TX/RX del USB): evítalos si usas el Monitor Serie |

### ESP32-C6

| GPIO | Problema |
|---|---|
| **24 – 29** | Flash SPI |
| **8, 9, 15** | Strapping (9 = BOOT, 8 = LED RGB en DevKitC-1) |
| **12, 13** | USB D− / D+ (USB-JTAG nativo) |
| 16, 17 | UART0 por defecto |

> Los **strapping pins** se pueden usar *después* del arranque, pero si el circuito los fuerza al nivel equivocado durante el reset, la placa no arranca o entra en modo descarga. En una clase con principiantes: no se usan.

---

## 4. Diferencias de comportamiento que confunden

| Situación | UNO | ESP32 |
|---|---|---|
| `delay(1000)` | Bloquea 1 s | Bloquea 1 s (pero hay 2 núcleos en WROOM) |
| `Serial.begin(9600)` | Estándar | Funciona, pero **115200** es lo habitual |
| Reinicio espontáneo | Raro | Común por `brownout` (fuente débil) |
| `int` | 2 bytes | **4 bytes** |
| `String` | Peligroso | Menos peligroso (hay más RAM) |
| Tiempo de compilación | 5 s | 30–90 s la primera vez |
| Consumo | ~50 mA | 80 mA en reposo, **picos de 500 mA** al transmitir Wi-Fi |

Ese último punto explica por qué el ESP32 a veces se reinicia: el Wi-Fi pide picos de corriente que un puerto USB débil no entrega. Con cable corto y buen puerto, no hay problema.

---

## 5. Reglas de oro para la sesión

1. **Nunca 5 V en un GPIO.**
2. **GND común** entre placa y cualquier fuente externa.
3. No usar GPIO6–11 (WROOM) ni 24–29 (C6).
4. No usar GPIO12 en WROOM. Nunca.
5. Resistencia de 220 Ω en cada LED.
6. Si la placa se reinicia sola: desconecta módulos y prueba solo con el LED.
7. Antes de mover cables, desconecta el USB.
