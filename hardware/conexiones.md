# 🔗 Conexiones de cada montaje

Criterio de colores:

- **Rojo** → `3V3` (nunca `5V` para un GPIO)
- **Negro / azul** → `GND`
- **Amarillo / verde** → señal

> Todo lo que va al ESP32 usa **3.3 V**. El pin `5V`/`VIN` del DevKit solo se usa como entrada de alimentación.

---

## 00 · Hola Serial

Sin cableado. Solo el cable USB.

---

## 01 · Blink

```
   GPIO21 ──[ 220 Ω ]──►|── GND
                     ánodo cátodo
```

| Elemento | Conexión |
|---|---|
| LED | ánodo → 220 Ω → **GPIO21**, cátodo → GND |

> El LED integrado no se usa: en el WROOM es GPIO2 y en el C6 es un RGB WS2812 en GPIO8.

---

## 02 · Semáforo

```
   GPIO21 ──[ 220 Ω ]──►|── GND      rojo
   GPIO22 ──[ 220 Ω ]──►|── GND      amarillo
   GPIO23 ──[ 220 Ω ]──►|── GND      verde
```

| LED | Pin | Resistencia |
|---|---|---|
| Rojo | GPIO21 | 220 Ω |
| Amarillo | GPIO22 | 220 Ω |
| Verde | GPIO23 | 220 Ω |

---

## 03 · PWM

```
   GPIO21 ──[ 220 Ω ]──►|── GND
```

Mismo montaje que Blink: solo cambia el código.

---

## 04 · Botón

```
   GPIO14 ──────┬──────[ pulsador ]────── GND
                │
         (pull-up interno)

   GPIO21 ──[ 220 Ω ]──►|── GND
```

| Elemento | Conexión |
|---|---|
| Pulsador | entre **GPIO14** y **GND** |
| Modo | `INPUT_PULLUP` (sin resistencia externa) |
| Lógica | `LOW` = presionado |

---

## 05 · Potenciómetro

```
        3V3 ──┬─────────────┐
              │             │
           ┌──┴──┐          │
           │ 10k │◄── cursor ──► GPIO32 (WROOM) / GPIO0 (C6)
           └──┬──┘          │
              │             │
             GND ───────────┘

   GPIO21 ──[ 220 Ω ]──►|── GND
```

| Pata del potenciómetro | Conexión |
|---|---|
| Extremo 1 | **3V3** (no 5 V) |
| Extremo 2 | GND |
| Cursor (central) | **GPIO32** en WROOM · **GPIO0** en C6 |

> Si conectas los extremos a 5 V, el cursor puede entregar 5 V al ADC y dañar el pin. Siempre a 3V3.

---

## 06 · Alerta de luz (integrador)

```
        3V3 ────┬────────────┐
                 │            │
              [ LDR ]      [ 10 kΩ ]
                 │            │
                 ├────────────┴──── GPIO32 (WROOM) / GPIO0 (C6)
                 │
                GND

   GPIO21 ──[ 220 Ω ]──►|── GND        LED de alarma
   GPIO22 ─────────────►| buzzer ── GND
```

| Elemento | Conexión |
|---|---|
| LDR | entre **3V3** y el pin analógico |
| Resistencia 10 kΩ | entre el pin analógico y GND |
| LED | GPIO21 con 220 Ω |
| Buzzer pasivo | GPIO22 |

Es un divisor de tensión: con más luz baja la resistencia del LDR y sube el voltaje leído.

---

## 07 · Wi-Fi

Sin cableado.

---

## Verificación antes de conectar el USB

1. ¿Algún GPIO recibe 5 V? **Debe ser 0 V.**
2. ¿Cada LED tiene su resistencia de 220 Ω?
3. ¿Todos los GND están unidos?
4. ¿El potenciómetro y el LDR están alimentados desde **3V3**, no desde 5 V?
5. ¿La placa está sobre una superficie aislante?
6. ¿El cable USB es de datos?

---

## Tabla resumen de pines

| Ejemplo | GPIO usados |
|---|---|
| 00 | — |
| 01 | 21 |
| 02 | 21, 22, 23 |
| 03 | 21 |
| 04 | 14, 21 |
| 05 | 32 (WROOM) / 0 (C6), 21 |
| 06 | 32 (WROOM) / 0 (C6), 21, 22 |
| 07 | — |
