# 📡 Monitoría: Introducción al ESP32

![ESP32](https://img.shields.io/badge/ESP32-WROOM--32%20%7C%20C6-E7352C?style=for-the-badge&logo=espressif&logoColor=white)
![Arduino IDE](https://img.shields.io/badge/Arduino%20IDE-2.x-00979D?style=for-the-badge&logo=arduino&logoColor=white)
![UNAL](https://img.shields.io/badge/UNAL-Introducci%C3%B3n%20a%20la%20Ingenier%C3%ADa%20Electr%C3%B3nica-8A1538?style=for-the-badge)
![Licencia](https://img.shields.io/badge/Licencia-MIT-green?style=for-the-badge)

## ¿Qué es?

Material de la monitoría **"Introducción al ESP32"** para estudiantes de **Introducción a la Ingeniería Electrónica** de la Universidad Nacional de Colombia, con **cero experiencia en programación**.

Sesión única de **3–4 horas** cuyo objetivo es que cada estudiante termine con:

- El **Arduino IDE configurado** con el core de Espressif y su placa reconocida.
- Un **LED parpadeando** y el **Monitor Serie** mostrando datos.
- Criterio para **no quemar la placa**: 3.3 V, pines prohibidos, strapping pins.
- **8 ejemplos de código** que funcionan tanto en **ESP32-WROOM-32** como en **ESP32-C6**.

> El ESP32 no es un Arduino UNO grande: es un SoC de 32 bits con Wi-Fi, BLE y 3.3 V. El material está escrito para que esa diferencia se entienda desde el primer montaje.

## ✨ Contenido

| Módulo | Qué encontrarás | Estado |
|---|---|---|
| 📥 Entorno | Arduino IDE 2.x + core `esp32` ≥ 3.0, drivers CH340/CP210x | ✅ |
| ⚡ Hardware | ESP32 vs UNO, 3.3 V, límites de corriente, pines peligrosos | ✅ |
| 📌 Pines | Tabla comparada WROOM-32 vs C6 y pines comunes de la monitoría | ✅ |
| 🗓️ Sesión | Guion minuto a minuto, plan B sin internet y checklist | ✅ |
| 🧪 Ejemplos | 8 sketches autocontenidos y comentados | ✅ |
| 🧰 Cheatsheet | Una página para tener al lado | ✅ |

## 🗺️ Flujo de la sesión

```
   ┌─────────────┐   ┌─────────────┐   ┌─────────────┐   ┌─────────────┐
   │ 1. CONTEXTO │──►│ 2. INSTALAR │──►│ 3. BLINK    │──►│ 4. E/S      │
   │ MCU, 3.3 V  │   │ IDE + core  │   │ setup/loop  │   │ digital/ADC │
   └─────────────┘   └─────────────┘   └─────────────┘   └──────┬──────┘
                                                                │
   ┌─────────────┐   ┌─────────────┐   ┌─────────────┐          │
   │ 7. CIERRE   │◄──│ 6. WiFi     │◄──│ 5. INTEGRA  │◄─────────┘
   │ repo, reto  │   │ scan (demo) │   │ LDR + alarma│
   └─────────────┘   └─────────────┘   └─────────────┘
```

## 📁 Estructura del repositorio

```
Monitoria_ESP32_Intro_IE/
├── README.md
├── LICENSE
├── .gitignore
├── docs/
│   ├── 01_entorno_arduino_ide.md   # instalación, core ESP32, drivers, errores
│   ├── 02_esp32_vs_uno.md          # 3.3 V, límites, strapping pins
│   ├── 03_pines_wroom_vs_c6.md     # tabla comparada de pines
│   ├── 04_plan_sesion.md           # guion minuto a minuto del monitor
│   ├── 05_entorno_esp_idf_vscode.md # flujo completo con VS Code + ESP-IDF
│   └── CHEATSHEET.md               # resumen de una página
├── ejemplos/
│   ├── 00_hola_serial/             # info del chip por Monitor Serie
│   ├── 01_blink/
│   ├── 02_semaforo/
│   ├── 03_pwm/
│   ├── 04_boton/
│   ├── 05_potenciometro/
│   ├── 06_alerta_luz/              # integrador con LDR
│   ├── 07_wifi_scan/               # demo de Wi-Fi
│   ├── 08_ultrasonico_led_proyecto/ # HC-SR04 + LED
│   ├── 09_ldr_led_proyecto/        # LDR + LED
│   └── 10_servo_barrera/           # HC-SR04 + servo
└── hardware/
    ├── lista_materiales.md
    └── conexiones.md
```

## 📌 Pines usados (idénticos en ambas placas)

| Función | GPIO | Por qué ese pin |
|---|---|---|
| LED 1 | **21** | Libre en WROOM y C6, soporta PWM |
| LED 2 | **22** | Libre en ambas |
| LED 3 | **23** | Libre en ambas |
| Pulsador | **14** | Libre en ambas, sin strapping |
| Entrada analógica | **32** (WROOM) / **0** (C6) | Único caso que difiere: ADC1 de cada chip |

Detalle y pines prohibidos en [`docs/03_pines_wroom_vs_c6.md`](docs/03_pines_wroom_vs_c6.md).

## 🔧 Hardware de la sesión

| Componente | Cantidad | Nota |
|---|---|---|
| ESP32-WROOM-32 o ESP32-C6 DevKitC-1 | 1 por estudiante | Ambas soportadas |
| Protoboard | 1 | — |
| LEDs (rojo, amarillo, verde) | 3 | Con resistencia de 220 Ω |
| Pulsador | 1 | Con `INPUT_PULLUP` |
| Potenciómetro 10 kΩ | 1 | Entrada analógica |
| LDR + resistencia 10 kΩ | 1 | Integrador |
| Buzzer pasivo | 1 | Integrador |
| Cables dupont | 20 | Macho-macho |
| Cable USB de datos | 1 | **No sirve uno de solo carga** |

Kit completo: ~$60.000 – $85.000 COP. Ver [`hardware/lista_materiales.md`](hardware/lista_materiales.md).

## 🚀 Cómo usar el repositorio

1. Sigue [`docs/01_entorno_arduino_ide.md`](docs/01_entorno_arduino_ide.md).
2. Abre `ejemplos/00_hola_serial/00_hola_serial.ino` y comprueba el Monitor Serie a **115200**.
3. Continúa en orden: cada ejemplo reutiliza lo anterior.
4. El guion de la sesión está en [`docs/04_plan_sesion.md`](docs/04_plan_sesion.md).

**¿Programas en VS Code con ESP-IDF?** No uses los `.ino`: sigue [`docs/05_entorno_esp_idf_vscode.md`](docs/05_entorno_esp_idf_vscode.md) y copia el `main.c` de cada ejemplo.

## ⚠️ Advertencia eléctrica

**GPIO a 3.3 V. 5 V en un pin destruye el chip.** No hay tolerancia a 5 V, a diferencia de muchos módulos de Arduino. Sensores de 5 V requieren divisor de tensión o level shifter. Ver [`docs/02_esp32_vs_uno.md`](docs/02_esp32_vs_uno.md).

## 🎓 Ruta de aprendizaje

| Fase | Tema | Ejemplo | Estado |
|---|---|---|---|
| 1 | Conocer el chip y el Monitor Serie | `00_hola_serial` | ✅ |
| 2 | Salida digital | `01_blink` | ✅ |
| 3 | Secuencias con varios LEDs | `02_semaforo` | ✅ |
| 4 | PWM: intensidad variable | `03_pwm` | ✅ |
| 5 | Entrada digital con antirrebote | `04_boton` | ✅ |
| 6 | ADC de 12 bits | `05_potenciometro` | ✅ |
| 7 | Integrador sensor + actuador | `06_alerta_luz` | ✅ |
| 8 | Wi-Fi (escaneo de redes) | `07_wifi_scan` | ✅ |

## 🧠 Stack

- **Hardware:** ESP32-WROOM-32 (Xtensa LX6, 240 MHz) · ESP32-C6 (RISC-V, 160 MHz)
- **Firmware:** Arduino core for ESP32 ≥ 3.0 (basado en ESP-IDF 5.x)
- **IDE:** Arduino IDE 2.x · `arduino-cli` opcional
- **Documentación:** Markdown

## Autor

**Yeison Dénnir Termal Cuastumal** — Ingeniería Electrónica, Universidad Nacional de Colombia 2026.

Licencia [MIT](LICENSE).
