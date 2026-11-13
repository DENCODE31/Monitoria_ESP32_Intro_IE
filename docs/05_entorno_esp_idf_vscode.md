# 🧰 ESP32 en VS Code con ESP-IDF

Guía para los grupos que programan el ESP32 **desde Visual Studio Code**, no desde Arduino IDE. Se usa la extensión oficial de Espressif (ESP-IDF) y un proyecto nuevo por práctica.

> El código de cada ejemplo está en dos formatos: `.ino` para Arduino IDE y `main.c` para ESP-IDF. Aquí se usa el `main.c`.

---

## 0. Antes de empezar

| Requisito | Cómo se comprueba |
|---|---|
| VS Code instalado | `code --version` o abrir el programa |
| Extensión **ESP-IDF** de Espressif | Panel de extensiones → buscar `ESP-IDF` → debe decir *Espressif Systems* |
| ESP-IDF instalado (v5.x o v6.x) | Al abrir la extensión, el panel muestra la versión detectada |
| Drivers USB | CH340 o CP2102, según el adaptador de la placa. En Linux suelen venir incluidos |
| Cable USB **de datos** | Si el puerto no aparece, el cable es de solo carga |

En Linux, el usuario debe pertenecer al grupo `dialout` para usar el puerto:

```bash
sudo usermod -aG dialout $USER    # cerrar sesion y volver a entrar
```

---

## 1. Crear la carpeta del proyecto

Crea una carpeta con el nombre de la práctica, por ejemplo `parqueadero_esp32`, y ábrela en VS Code con **Archivo → Abrir carpeta**.

No importa que esté vacía: el asistente crea adentro la estructura del proyecto.

---

## 2. Activar la extensión

1. En la barra lateral, clic en el icono de **ESP-IDF** (logo de la tarjeta).
2. Si aparece el aviso *"No standard ESP-IDF project was found..."*, pulsa **Activate Anyway**. Es normal: la carpeta todavía no es un proyecto.

---

## 3. Abrir el asistente de proyecto

Dos caminos, el segundo es el más rápido:

- Panel lateral → **Commands → Advanced → New Project Wizard**.
- `Ctrl+Shift+P` → escribir **ESP-IDF: New Project Wizard**.

En **Select framework to use** elige la versión instalada (por ejemplo `ESP-IDF v5.4` o `v6.1`).

---

## 4. Configurar el proyecto

| Campo | Qué poner |
|---|---|
| Plantilla (izquierda) | `sample_project` |
| Project Name | nombre de la práctica, por ejemplo `barrera_parqueadero` |
| Enter Project Directory | la carpeta del paso 1 |
| Choose ESP-IDF Target | `esp32` (WROOM-32), `esp32c6` o `esp32s3` según la placa |
| Choose ESP-IDF Board | si aparece, el modelo; si no aparece, se deja por defecto |
| Choose Serial Port | `COM3` (Windows), `/dev/ttyUSB0` o `/dev/ttyACM0` (Linux) |

Pulsa **Create Project** y luego **Open Project** en el aviso.

El target es el **chip**, no la placa: `esp32`, `esp32c6`, `esp32s3`. Si te equivocas, se corrige después con `idf.py set-target`.

---

## 5. Estructura generada

```
barrera_parqueadero/
├── .devcontainer/
├── .vscode/
├── main/
│   ├── main.c          <- aqui va el codigo del ejemplo
│   └── CMakeLists.txt
├── CMakeLists.txt
└── sdkconfig
```

---

## 6. Copiar el código del ejemplo

1. Abre `main/main.c`: trae un `app_main` de ejemplo que solo imprime mensajes.
2. En GitHub, entra a `ejemplos/`, abre la carpeta del ejemplo y su `main.c`.
3. Botón **Copy raw file** (arriba a la derecha del visor).
4. En VS Code: borra todo el contenido de `main/main.c`, pega y guarda con `Ctrl+S`.

Un proyecto ESP-IDF tiene **un solo** `main.c`. Si pegas dos ejemplos, el segundo reemplaza al primero: no se suman.

| Práctica | Archivo a copiar |
|---|---|
| Blink | `ejemplos/01_blink/main.c` |
| Semáforo | `ejemplos/02_semaforo/main.c` |
| PWM | `ejemplos/03_pwm/main.c` |
| Botón | `ejemplos/04_boton/main.c` |
| Potenciómetro | `ejemplos/05_potenciometro/main.c` |
| LDR + alarma | `ejemplos/06_alerta_luz/main.c` |
| Wi-Fi | `ejemplos/07_wifi_scan/main.c` |
| Ultrasónico + LED | `ejemplos/08_ultrasonico_led_proyecto/main.c` |
| LDR + LED | `ejemplos/09_ldr_led_proyecto/main.c` |
| Servo + barrera | `ejemplos/10_servo_barrera/main.c` |
| Servo + buzzer | `ejemplos/11_servo_buzzer/main.c` |

---

## 7. Compilar, subir y monitorear

Con la placa conectada, usa la barra azul inferior de VS Code:

| Icono | Acción | Equivalente |
|---|---|---|
| 🔥 Llamita | **Build, Flash and Monitor** | todo en uno |
| ⚙️ Engranaje | **Build** | `idf.py build` |
| ⚡ Rayo | **Flash** | `idf.py -p PUERTO flash` |
| 💻 Monitor | **Monitor** | `idf.py -p PUERTO monitor` |

La primera compilación tarda varios minutos: genera cientos de archivos de componentes. Las siguientes son rápidas.

Debe terminar con:

```
Project build complete.
...
Flash Done
```

El monitor abre a **115200 baudios**. Para salir del monitor: `Ctrl+]`.

Si solo cambiaste el código, la llamita hace las tres cosas. Si cambiaste de placa o de target, primero **Full Clean** y vuelve a compilar.

---

## 8. Errores frecuentes

| Síntoma | Causa | Solución |
|---|---|---|
| `Failed to connect to ESP32: No serial data received` | La placa no entró en modo descarga | Mantén **BOOT**, pulsa **EN**, suelta BOOT y reintenta |
| `Permission denied: /dev/ttyUSB0` | Falta el grupo `dialout` | `sudo usermod -aG dialout $USER` y reinicia sesión |
| `Could not open port ... Resource busy` | El monitor sigue abierto | Cierra el monitor (`Ctrl+]`) antes de flashear |
| No aparece ningún puerto | Cable de solo carga o driver ausente | Cambia el cable; instala CH340/CP2102 |
| Compila pero no pasa nada | Target equivocado | `idf.py set-target esp32` (o `esp32c6` / `esp32s3`) |
| `undefined reference to app_main` | Pegaste un `.ino` en vez del `main.c` | Copia el archivo `main.c` de la misma carpeta |
| El LED o el sensor no responde | Falta `#define HELTEC_V3` en placas Heltec | Agrégalo tras los `#include` |
| Monitor con basura | Baudaje incorrecto | 115200 |

---

## 9. Diferencia con Arduino IDE (para no confundirse)

| Arduino IDE | ESP-IDF |
|---|---|
| `setup()` y `loop()` | solo `app_main()`, con `while (1)` |
| `pinMode` / `digitalWrite` | `gpio_set_direction` / `gpio_set_level` |
| `delay()` | `vTaskDelay(pdMS_TO_TICKS())` |
| `millis()` | `esp_timer_get_time() / 1000` |
| `Serial.print` | `printf` |
| `analogRead` / `analogWrite` | driver ADC / driver LEDC |

La tabla completa está en [`ejemplos/README.md`](../ejemplos/README.md).
