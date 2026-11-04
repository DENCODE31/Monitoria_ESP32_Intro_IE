# 📥 Entorno: Arduino IDE para ESP32

Objetivo: **20 minutos por computador** desde cero hasta el LED parpadeando. Si algo falla, la tabla de errores del final cubre el 90 % de los casos.

---

## 1. Instalar el IDE

Descarga **Arduino IDE 2.x** desde el sitio oficial: <https://www.arduino.cc/en/software>

| Sistema | Archivo |
|---|---|
| Windows 10/11 | `arduino-ide_x.y.z_Windows_64bit.exe` |
| Linux x86_64 | `arduino-ide_x.y.z_Linux_64bit.AppImage` |
| macOS | `.dmg` (Intel o Apple Silicon) |

En Linux:

```bash
chmod +x arduino-ide_*_Linux_64bit.AppImage
./arduino-ide_*_Linux_64bit.AppImage
```

---

## 2. Agregar el soporte de ESP32 (paso que casi todos olvidan)

El IDE **no** trae ESP32 de fábrica. Hay que decirle de dónde descargarlo:

1. Abre **File → Preferences** (`Ctrl + ,`).
2. En **Additional boards manager URLs** pega:

```
https://espressif.github.io/arduino-esp32/package_esp32_index.json
```

3. Acepta con **OK**.
4. Ve a **Tools → Board → Boards Manager**, busca `esp32` e instala **esp32 by Espressif Systems**.

> ⚠️ **Versión mínima 3.0.0.** Las versiones 2.x no soportan ESP32-C6. La descarga pesa ~1.5 GB: en la universidad puede tardar 15–30 min. Llévala precargada (ver plan B en `04_plan_sesion.md`).

---

## 3. Seleccionar la placa

**Tools → Board → esp32:**

| Tu placa | Opción en el IDE |
|---|---|
| ESP32-WROOM-32 (DevKit V1, 30 o 38 pines) | **ESP32 Dev Module** |
| ESP32-C6 DevKitC-1 | **ESP32C6 DevKitC-1** |
| ESP32-S3 DevKitC-1 | **ESP32S3 DevKitC-1** |

Ajustes recomendados para WROOM-32:

| Opción | Valor |
|---|---|
| Upload Speed | 921600 (si falla, 115200) |
| Flash Frequency | 80 MHz |
| Flash Size | 4 MB (32 Mb) |
| Partition Scheme | Default 4 MB with spiffs |
| Core Debug Level | None |

Para C6 no hay que tocar nada: los valores por defecto funcionan.

**Puerto:** **Tools → Port**

| Placa / chip USB | Puerto Linux | Puerto Windows |
|---|---|---|
| WROOM con CH340 | `/dev/ttyUSB0` | `COM3`, `COM4`… |
| WROOM con CP2102 | `/dev/ttyUSB1` | `COMx` |
| C6 DevKitC-1 (USB nativo) | `/dev/ttyACM0` | `COMx` |

---

## 4. Drivers USB

| Chip | Dónde aparece | Driver |
|---|---|---|
| **CH340 / CH341** | Clones económicos de WROOM | WCH `CH341SER` |
| **CP2102** | Placas NodeMCU / DevKit buenas | Silicon Labs CP210x |
| **USB-JTAG nativo** | ESP32-C6, S3, C3 | Incluido en el sistema |

### Windows

Administrador de dispositivos → **Puertos (COM y LPT)**. Si aparece `USB2.0-Serial` con signo amarillo, falta el driver CH340. Instálalo, reconecta y verifica.

### Linux

```bash
sudo usermod -aG dialout $USER    # cerrar sesión y volver a entrar
groups | grep dialout             # debe mostrar dialout
ls /dev/ttyUSB* /dev/ttyACM*      # con la placa conectada
dmesg | tail -20                  # busca CH341, CP210x o cdc_acm
```

Regla udev para CH340 (si el puerto aparece y desaparece):

```bash
echo 'SUBSYSTEM=="tty", ATTRS{idVendor}=="1a86", ATTRS{idProduct}=="7523", MODE="0666"' \
  | sudo tee /etc/udev/rules.d/99-ch340.rules
sudo udevadm control --reload-rules && sudo udevadm trigger
```

---

## 5. Primer programa

1. Abre `ejemplos/00_hola_serial/00_hola_serial.ino`.
2. Verifica **placa** y **puerto**.
3. **Upload (→)**. En la consola debe aparecer `Connecting........_____` y luego `Hash of data verified`.
4. Abre el **Monitor Serie** a **115200 baudios**.

Salida esperada (ejemplo en C6):

```
Chip: ESP32-C6
Revision: 0
Frecuencia: 160 MHz
Heap libre: 380000 bytes
```

---

## 6. Errores comunes y solución

| Mensaje | Causa | Solución |
|---|---|---|
| `Failed to connect to ESP32: Timed out waiting for packet header` | La placa no entró en modo descarga | Mantén **BOOT** presionado, pulsa **EN/RST**, suelta BOOT y sube |
| `A fatal error occurred: Could not open port` | Puerto ocupado | Cierra el Monitor Serie u otro IDE |
| `Permission denied: '/dev/ttyUSB0'` | Falta grupo `dialout` | `sudo usermod -aG dialout $USER` + reiniciar sesión |
| `Board not found: esp32c6` | Core ESP32 < 3.0 | Actualiza `esp32 by Espressif Systems` a 3.x |
| `Sketch uses 1200000 bytes (91%)` | Partición por defecto pequeña | Partition Scheme → `Huge APP (3MB No OTA)` |
| Monitor Serie en blanco o basura | Baudaje distinto | Debe ser **115200** |
| La placa reinicia en bucle | Consumo excesivo o corto | Desconecta todo, alimenta solo por USB |
| `Brownout detector was triggered` | Caída de tensión (servos, LEDs de potencia) | Fuente externa 5 V con GND común |
| No aparece ningún puerto | Cable de solo carga | Cambia el cable por uno de datos |

**Truco del modo descarga** (sirve en cualquier placa sin botón BOOT accesible): mantén el pin **GPIO0/BOOT a GND** al momento de resetear, suelta y sube el programa.

---

## 7. Checklist de salida

- [ ] Core `esp32` instalado (versión 3.x).
- [ ] Placa correcta seleccionada (Dev Module o C6 DevKitC-1).
- [ ] Puerto visible y seleccionado.
- [ ] `00_hola_serial` cargado y Monitor Serie a 115200 mostrando datos.
- [ ] LED externo en GPIO21 parpadeando con `01_blink`.
