# Ready-made firmware (greenhouse)

Built for the **Wemos D1 mini32 (ESP32-WROOM, 4 MB flash)**, Arduino-ESP32 core 2.0.17,
default partition table. The web page (`data/`) is included in `spiffs.bin`.

## Update (keeps your settings: wifi, targets, calibration)

```
esptool.py --chip esp32 --baud 921600 write_flash 0x10000 firmware.bin 0x290000 spiffs.bin
```

## Clean install (new board, or when something is badly broken)

One file contains everything (bootloader, partition table, firmware, web page). It erases
everything, including the saved settings. There are two copies of the same image; use the one
whose name matches the address you flash it to:

| File | Flash address |
| --- | --- |
| `greenhouse-full-flash-0x1000.bin` | `0x1000` (the default address in the esptool web page) |
| `greenhouse-full-flash-0x0.bin` | `0x0` |

```
esptool.py --chip esp32 --baud 921600 write_flash 0x1000 greenhouse-full-flash-0x1000.bin
```

or, the same with the separate files:

```
esptool.py --chip esp32 --baud 921600 write_flash 0x1000 bootloader.bin 0x8000 partitions.bin 0xe000 boot_app0.bin 0x10000 firmware.bin 0x290000 spiffs.bin
```

A wrong address cannot damage the ESP32: it just won't start. Connect again and flash the
right file at the right address.

No Python? Use the browser flasher https://espressif.github.io/esptool-js/ (Chrome/Edge,
USB cable): *Connect*, keep Flash Address `0x1000`, choose `greenhouse-full-flash-0x1000.bin`
and press *Program*. If no COM port shows up, install the CP210x or CH340 USB driver; if
connecting fails, hold the BOOT button while clicking *Connect*.

## Over wifi (no cable)

Open `http://<controller-ip>/update` (or `http://192.168.4.1/update` on the ESP32-AP network).
Upload `firmware.bin` with *OTA Mode: Firmware*, wait for the restart, then upload `spiffs.bin`
with *OTA Mode: Filesystem*. The saved settings are kept. Never upload a *full-flash* file there.

Controllers still running an older firmware show the older update page (two round buttons
*Firmware* / *Filesystem* instead of the *OTA Mode* list); it works the same.
