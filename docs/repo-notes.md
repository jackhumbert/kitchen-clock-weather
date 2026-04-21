# Repo Notes

## Verified Findings

### Hardware

- Board target: Waveshare ESP32-S3-Touch-AMOLED-1.75-B.
- Display controller: CO5300 over QSPI.
- Touch controller: CST9217 over I2C.
- RTC: PCF85063.
- Screen size: 466 x 466.
- MCU resources: ESP32-S3R8, 8 MB PSRAM, 16 MB flash.

### Firmware Stack

- PlatformIO.
- Arduino framework.
- LVGL 9.x.
- Arduino_GFX for display output.
- ArduinoJson for Open-Meteo parsing.

### Confirmed Pin Map

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

## Build Notes

- The project builds successfully with `pio run`.
- Generate IntelliSense metadata with `pio run -t compiledb`.
- `compile_commands.json` is intentionally local-only and ignored.

## Framework Quirk

- With PlatformIO `espressif32` and Arduino `3.1.0` in this repo, the active framework package's `libraries/Network/src` directory must be added to the include path.
- That workaround lives in `scripts/framework_workarounds.py`.
- The workaround must use the active framework package returned by PlatformIO.
- Do not scan all installed framework packages. Doing that can mix incompatible `NetworkEvents` header versions and cause unresolved `postEvent` linker errors.

## Documentation Rule

When a new fact is both verified and likely to help future agents or users:

1. Update this file with the concise technical finding.
2. Update `README.md` if the finding changes user-facing setup, build, or hardware expectations.
3. Update `docs/firmware-plan.md` if the finding changes architecture or implementation order.
4. Keep notes short, verified, and specific.

Examples of findings worth documenting:

- Build or linker quirks.
- Confirmed hardware pins or controller IDs.
- Reliable flash, upload, or monitor commands.
- Constraints discovered during hardware bring-up.
- API limitations that affect implementation choices.