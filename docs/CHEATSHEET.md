# 🧰 Cheatsheet — ESP32 con Arduino IDE

Una página para tener al lado durante la sesión.

---

## Placa y pines

| Función | WROOM-32 | ESP32-C6 |
|---|---|---|
| LED 1 | GPIO21 | GPIO21 |
| LED 2 | GPIO22 | GPIO22 |
| LED 3 | GPIO23 | GPIO23 |
| Pulsador | GPIO14 | GPIO14 |
| Analógico | **GPIO32** | **GPIO0** |
| Placa en el IDE | ESP32 Dev Module | ESP32C6 DevKitC-1 |
| Puerto Linux | `/dev/ttyUSB0` | `/dev/ttyACM0` |

**Prohibidos — WROOM:** 6–11 (flash), 12 (peligro), 34–39 (solo entrada), 1/3 (USB).
**Prohibidos — C6:** 24–29 (flash), 8/9/15 (strapping), 12/13 (USB).

---

## Voltaje

```
Lógica: 3.3 V        ¡5 V en un GPIO = chip muerto!
LED: 220 Ω a GND
Corriente por pin: 20 mA recomendado
Consumo Wi-Fi: picos de ~500 mA
```

---

## Estructura

```cpp
void setup() {
    Serial.begin(115200);
    pinMode(21, OUTPUT);
}

void loop() {
}
```

---

## Funciones clave

```cpp
pinMode(pin, OUTPUT | INPUT | INPUT_PULLUP);
digitalWrite(pin, HIGH | LOW);
digitalRead(pin);

analogRead(pin);                 // 0..4095 (12 bits)
analogWrite(pin, 0..255);        // PWM (LEDC)
map(x, 0, 4095, 0, 255);
constrain(x, min, max);

delay(ms);
millis();

Serial.begin(115200);
Serial.print(x);  Serial.println(x);
Serial.available();  Serial.read();
```

---

## Info del chip

```cpp
Serial.println(ESP.getChipModel());    // ESP32-C6 / ESP32-D0WDQ6
Serial.println(ESP.getCpuFreqMHz());
Serial.println(ESP.getFreeHeap());
```

---

## Errores del día

| Síntoma | Causa |
|---|---|
| `Timed out waiting for packet header` | Falta modo descarga: BOOT + RST |
| No aparece el puerto | Cable de carga o driver CH340 |
| Monitor Serie mudo/basura | Baudaje ≠ 115200 |
| Reinicio en bucle | `brownout`: consumo alto o corto |
| Compila pero no hace nada | Placa equivocada en Tools → Board |
| Botón cuenta doble | Rebote: falta antirrebote de 30 ms |
| ADC siempre 0 o 4095 | Pin equivocado (32 en WROOM, 0 en C6) |

---

## Antirrebote

```cpp
if (digitalRead(14) == LOW && millis() - tUltimo > 30) {
    tUltimo = millis();
}
```

## Tarea periódica sin `delay()`

```cpp
if (millis() - tUltimo >= 500) {
    tUltimo = millis();
}
```

## Conexión mínima de un LED

```
GPIO21 ──[ 220 Ω ]──►|── GND
                  ánodo cátodo
```
