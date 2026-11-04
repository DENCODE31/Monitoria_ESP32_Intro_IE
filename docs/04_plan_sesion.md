# 🗓️ Guion de la sesión (3–4 horas)

Material para el monitor. Sigue el orden, respeta los tiempos y usa los recortes si te retrasas.

---

## Antes de la sesión (la semana previa)

- [ ] Confirmar placa de cada estudiante (WROOM-32 o C6) por correo.
- [ ] Enviar instrucciones: **portátil propio, cable USB de datos, placa cargada**.
- [ ] Descargar el instalador de Arduino IDE y el core `esp32` en un USB de 4 GB (plan B).
- [ ] Imprimir `docs/CHEATSHEET.md` (1 por estudiante).
- [ ] Probar un montaje completo con cada modelo de placa.
- [ ] Llevar 2 placas de repuesto, cables y resistencias extra.
- [ ] Reservar un aula con enchufes y proyector.

> Si el grupo trae 4 placas distintas, prepara la tabla de pines abierta en el proyector desde el inicio.

---

## Cronograma de 4 horas

### 0:00 – 0:15 · Contexto

**Objetivo:** que sepan qué tienen en la mano y por qué no es un Arduino UNO.

- Pregunta de apertura: *"¿Qué creen que hace un microcontrolador?"*
- Cuadro en el tablero:

| | Arduino UNO | ESP32 |
|---|---|---|
| Voltaje | 5 V | **3.3 V** |
| CPU | 16 MHz, 8 bits | 240 MHz, 32 bits |
| Wi-Fi | No | Sí |

- Demo del monitor: LED + potenciómetro del ejemplo integrador. Que vean a dónde llegan.
- Advertencia que hay que repetir tres veces hoy: **5 V en un GPIO quemado el chip**.

### 0:15 – 0:55 · Instalación

**Objetivo:** IDE instalado, core `esp32` 3.x, placa y puerto correctos.

Pasos en pantalla (todos a la vez, tú rotando):

1. Instalar Arduino IDE 2.x.
2. **Preferences → Additional boards manager URLs**: pegar la URL de Espressif.
3. **Boards Manager**: instalar `esp32 by Espressif Systems` (tarda 10–25 min).
4. Seleccionar placa y puerto.
5. Abrir `ejemplos/00_hola_serial`.

Mientras descargan, explica con la placa en la mano:

- 3.3 V vs 5 V, y por qué un LED necesita 220 Ω.
- Qué es un GPIO y qué significa "salida" y "entrada".
- Los pines que no se tocan (6–11 en WROOM, 24–29 en C6).

**Si alguien termina antes:** pídele que cambie el mensaje del `Serial.println` y lo vuelva a subir.

### 0:55 – 1:25 · Blink

**Objetivo:** `setup()` / `loop()`, `pinMode`, `digitalWrite`, `delay`.

```cpp
pinMode(21, OUTPUT);
digitalWrite(21, HIGH);
delay(500);
```

- Montaje: LED en GPIO21 con 220 Ω a GND.
- Retos rápidos: 100 ms (¿qué pasa?), dos LEDs alternados, patrón de tres tiempos.
- Cierra el Monitor Serie cuando subas código nuevo (error clásico).

### 1:25 – 1:30 · Pausa

Aprovecha para revisar quién quedó atrás. Sin pausa, la segunda mitad se cae.

### 1:30 – 2:05 · Salidas: semáforo y PWM

**Objetivo:** varios actuadores y `analogWrite`.

- Ejemplo `02_semaforo`: 3 LEDs, tiempos distintos, funciones propias.
- Ejemplo `03_pwm`: el LED "respira". Explica duty cycle con el símil del interruptor que abre y cierra 1000 veces por segundo.
- Error a anticipar: LED en un pin sin PWM (en el ESP32 casi todos sirven, pero si usan 34–39 en WROOM no hay salida).

### 2:05 – 2:45 · Entradas

**Objetivo:** leer el mundo y reaccionar.

- Ejemplo `04_boton`: `INPUT_PULLUP` → **LOW significa presionado**. Antirrebote con `millis()`.
  - Todos van a reportar "cuenta 2 o 3 por pulsación" si no se explica el rebote. Es la mejor lección del día.
- Ejemplo `05_potenciometro`: ADC de 12 bits (0–4095), `map`, Monitor Serie.
  - Aquí se ve la diferencia de pines: GPIO32 en WROOM, GPIO0 en C6.
- Reto: LED que se enciende cuando el potenciómetro pasa de la mitad.

### 2:45 – 3:25 · Integrador: alerta de luz

**Objetivo:** unir sensor + decisión + actuador.

- Ejemplo `06_alerta_luz`: LDR + LED + buzzer, con umbral e histéresis.
- Guía paso a paso, no copies el código completo: dicta el umbral, deja que ellos lo calibren con el Monitor Serie.
- Pregunta de cierre: *"¿por qué sin histéresis el buzzer tiembla?"*

### 3:25 – 3:50 · Wi-Fi (solo si vas al día)

**Objetivo:** demostrar que el hardware sirve para IoT.

- Ejemplo `07_wifi_scan` **copiado del repo, no escrito por ellos**. Son 10 líneas de configuración que aún no entienden.
- Muestra los SSID y la intensidad de señal. Compara con el celular.
- Si no hay tiempo: se salta sin culpa. Está en el repo para que lo corran en casa.

### 3:50 – 4:00 · Cierre

- Recapitula los 5 conceptos: 3.3 V, GPIO, `setup/loop`, entrada vs salida, `millis()`.
- Entrega el repo: <https://github.com/DENCODE31/Monitoria_ESP32_Intro_IE>
- Reto para casa: semáforo con botón peatonal (el botón acelera el cambio).
- Encuesta de 1 minuto: qué quedó claro, qué no.

---

## Versión de 3 horas

| Recorte | Acción |
|---|---|
| Wi-Fi | Se elimina (3:25–3:50). |
| Integrador | Solo LDR + LED, sin buzzer. |
| PWM | Se une al bloque de semáforo, sin retos extra. |
| Instalación | Se envía como guía previa y solo se resuelven problemas en clase. |

---

## Roles y dinámica

| Momento | Dinámica |
|---|---|
| Instalación | Individual, monitor rotando cada 5 min |
| Blink y salidas | Individual |
| Entradas | Parejas: uno cablea, otro escribe |
| Integrador | Individual, con el monitor validando el umbral |

Regla de convivencia: **nadie sube código de otro sin entenderlo**. Si alguien pide ayuda, primero explica qué esperaba que pasara.

---

## Plan B sin internet

1. Instalador de Arduino IDE en USB.
2. Carpeta `packages` del core `esp32` precargada:
   - Linux: `~/.arduino15/packages/esp32`
   - Windows: `%LOCALAPPDATA%\Arduino15\packages\esp32`
3. Copiar esa carpeta en cada equipo **con el IDE cerrado**.
4. Los drivers CH340/CP210x también en el USB.
5. Todos los ejemplos en el USB, en una carpeta `Monitoria_ESP32`.

---

## Solución de problemas en vivo

| Síntoma | Acción inmediata |
|---|---|
| `Timed out waiting for packet header` | Mantener BOOT, pulsar RST, soltar BOOT, subir |
| Puerto no aparece | Cambiar cable; revisar `dmesg` |
| Monitor Serie mudo | Baudaje 115200 |
| Reinicio en bucle (`brownout`) | Desconectar módulos, probar solo USB |
| Compila para la placa equivocada | Tools → Board → revisar modelo |
| El LED no enciende | Polaridad: pata larga (ánodo) al pin, corta a GND |

---

## Métricas de éxito de la sesión

- **100 %** de los estudiantes con `00_hola_serial` funcionando.
- **≥ 75 %** con el integrador funcionando al final.
- **0** placas dañadas.
- Todos saben responder: *"¿por qué no puedo conectar 5 V a un GPIO?"*
