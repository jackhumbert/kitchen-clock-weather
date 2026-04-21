# Repo Notes

## Verified Findings

### Hardware

- Board target: Waveshare ESP32-S3-Touch-AMOLED-1.75-B.
- Display controller: CO5300 over QSPI.
- Touch controller: CST9217 over I2C.
- RTC: PCF85063.
- Screen size: 466 x 466.
- MCU resources observed on the intended board during upload: 8 MB embedded PSRAM and 16 MB embedded flash.
- The earlier `Embedded Flash 4MB (XMC)` and `Embedded PSRAM 2MB (AP_3v3)` readings were from a different ESP32 that was attached at the same time.

### Firmware Stack

- PlatformIO.
- Arduino framework.
- LVGL 9.x.
- Arduino_GFX for display output.
- ArduinoJson for Open-Meteo parsing.
- SensorLib for CST9217 touch.

### Touch Notes

- SensorLib is available from the PlatformIO registry as `lewisxhe/SensorLib`.
- The vendor LVGL widget example for this board configures the CST9217 with `setMaxCoordinates(466, 466)` and `setMirrorXY(true, true)`.
- The current firmware reads touch points in the LVGL input callback instead of depending on `TP_INT` edge handling.

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
- Keep the default `esp32s3_120_16_8-qio_opi` board geometry for this repo; the intended board really is a 16 MB flash / 8 MB PSRAM target.
- If multiple ESP32-class USB devices are attached, PlatformIO can auto-detect the wrong port. Prefer `pio run -t upload --upload-port <port>` and target the Espressif native USB JTAG/serial device with VID:PID `303A:1001`.

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