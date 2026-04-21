# kitchen-clock-weather

Firmware for the Waveshare ESP32-S3-Touch-AMOLED-1.75-B using PlatformIO, Arduino, and LVGL 9.x.

## Current Status

- PlatformIO project scaffolds and builds successfully.
- Display bring-up works through Arduino_GFX and LVGL 9.
- CST9217 touch input is wired into LVGL through SensorLib.
- Wi-Fi, NTP time sync, EST/EDT timezone handling, and Open-Meteo weather fetch are wired.
- The main UI is a functional first-pass clock and weather screen.

Not implemented yet:

- PCF85063 RTC fallback.
- Final weather icon assets.
- Full on-device validation beyond initial bring-up.

## Board Notes

Verified for the ESP32-S3-Touch-AMOLED-1.75-B:

- Display: CO5300 over QSPI.
- Touch: CST9217 over I2C.
- Resolution: 466 x 466.
- MCU: ESP32-S3 with 8 MB PSRAM and 16 MB flash.
- The intended board reports 8 MB PSRAM and 16 MB flash during upload. The earlier 2 MB / 4 MB readings came from a different ESP32 that was attached at the same time.

Verified pin mappings from vendor examples:

- `LCD_SDIO0=4`
- `LCD_SDIO1=5`
- `LCD_SDIO2=6`
- `LCD_SDIO3=7`
- `LCD_SCLK=38`
- `LCD_CS=12`
- `LCD_RESET=39`
- `IIC_SDA=15`
- `IIC_SCL=14`
- `TP_INT=11`
- `TP_RESET=40`

## Setup

1. Copy `include/secrets.example.h` to `include/secrets.h`.
2. Fill in Wi-Fi credentials and weather coordinates.
3. Build with PlatformIO.

## Useful Commands

```bash
pio run
pio run -t compiledb
pio run -t upload
pio device monitor -b 115200
```

## Repo Conventions

- `include/secrets.h` is local-only and must never be committed.
- `compile_commands.json` is generated locally and should not be committed.
- Verified build and environment notes live in `docs/repo-notes.md`.
- Firmware planning and architecture notes live in `docs/firmware-plan.md`.

## Build Quirk

This PlatformIO and Arduino framework combination needs the active framework package's `libraries/Network/src` include path added explicitly at build time. That is handled by `scripts/framework_workarounds.py`.

Do not broaden that workaround to scan all installed framework packages. Mixing headers from multiple installed Arduino core snapshots causes `NetworkEvents::postEvent` signature mismatches and linker failures.

## Upload Port Quirk

If multiple ESP32 devices are attached over USB, do not rely on PlatformIO's auto-detected upload port. The target board appears as Espressif's native USB JTAG/serial device with VID:PID `303A:1001`, and flashing the wrong ESP32 can lead to misleading hardware readings and boot failures.

Use an explicit upload port when more than one serial device is present:

```bash
pio run -t upload --upload-port /dev/cu.usbmodem23101
```