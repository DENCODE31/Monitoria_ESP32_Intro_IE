# 🛒 Lista de materiales

Precios de referencia en **COP** (Colombia, 2026). Varían según proveedor y ciudad.

---

## Kit por estudiante

| # | Componente | Cant. | Precio unit. aprox. | Subtotal | Nota |
|---|---|---|---|---|---|
| 1 | ESP32-WROOM-32 DevKit (clon) | 1 | $25.000 – $35.000 | $25.000 – $35.000 | CH340 o CP2102 |
| 1b | *Alternativa:* ESP32-C6 DevKitC-1 | 1 | $45.000 – $60.000 | — | Wi-Fi 6 + Zigbee/Thread |
| 2 | Protoboard 400 puntos | 1 | $8.000 – $12.000 | $8.000 – $12.000 | — |
| 3 | LEDs 5 mm surtidos | 10 | $300 | $3.000 | Rojo, amarillo, verde |
| 4 | Resistencias 220 Ω | 10 | $100 | $1.000 | LEDs |
| 5 | Resistencias 10 kΩ | 5 | $100 | $500 | Divisor del LDR |
| 6 | Pulsadores 6×6 mm | 2 | $800 | $1.600 | — |
| 7 | Potenciómetro 10 kΩ lineal | 1 | $2.000 | $2.000 | B10K |
| 8 | LDR GL5528 | 1 | $1.000 | $1.000 | Integrador |
| 9 | Buzzer pasivo 3.3–5 V | 1 | $1.500 | $1.500 | Pasivo para `tone()` |
| 10 | Cables dupont macho-macho | 20 | — | $8.000 | — |
| 11 | Cable USB de datos | 1 | $5.000 | $5.000 | **No sirve el de carga** |

**Total con WROOM-32: ~$56.000 – $72.000 COP por estudiante.**
Con C6 DevKitC-1: ~$76.000 – $97.000 COP.

---

## Material del monitor

| Componente | Para qué |
|---|---|
| 2 placas de repuesto | Reemplazo inmediato si una falla |
| Multímetro | Medir 3.3 V y continuidad |
| USB con instalador, core `esp32` y drivers | Plan B sin internet |
| Resistencias y LEDs extra | Se pierden o se queman |
| Protoboard armado de demostración | Mostrar el resultado final |
| Tarjetas impresas del cheatsheet | Una por estudiante |

---

## Opcionales para extender la sesión

| Componente | Precio aprox. | Práctica |
|---|---|---|
| Sensor DHT11 / DHT22 | $4.000 / $12.000 | Temperatura y humedad por Wi-Fi |
| Pantalla OLED SSD1306 I²C | $10.000 | Mostrar datos sin PC |
| Módulo relé 5 V | $6.000 | Control de cargas (requiere fuente externa) |
| Servo SG90 | $6.000 | PWM y ángulos |
| Tira WS2812B (10 LED) | $12.000 | LED RGB direccionable |

---

## Dónde comprar en Colombia

| Proveedor | Ventaja | Desventaja |
|---|---|---|
| Tiendas de electrónica locales | Probar antes de pagar | Sin garantía formal |
| Mercado Libre | Envío y variedad | Calidad variable |
| Distribuidores especializados | Factura y soporte | Más caro |
| AliExpress | Precio más bajo | 3–6 semanas de espera |

> Para la sesión alcanza con placa, protoboard, 3 LEDs, 3 resistencias de 220 Ω, un pulsador y un potenciómetro. Lo demás es para el integrador.

---

## Cómo reconocer una placa decente

- Chip USB **CH340** o **CP2102** visible y con driver disponible.
- Regulador AMS1117 y capacitor cerca del módulo.
- Serigrafía clara: `ESP32-WROOM-32` o `ESP32-C6-WROOM-1`.
- Botones **BOOT** y **EN/RST** presentes (esenciales para recuperar una placa).
- Que el vendedor permita probarla con un cable USB antes de pagar.
