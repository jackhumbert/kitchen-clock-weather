# Copilot Instructions

This repo targets the Waveshare ESP32-S3-Touch-AMOLED-1.75-B using PlatformIO, Arduino, LVGL 9, and Arduino_GFX.

## Workflow

- Build with `pio run` after meaningful code changes.
- Regenerate `compile_commands.json` with `pio run -t compiledb` when build flags or include paths change.
- Keep changes focused and minimal.
- Do not read, modify, or commit `include/secrets.h` unless the user explicitly asks.

## Documentation Policy

When you learn something novel and verified that would help future agents or users:

1. Add the concise technical note to `docs/repo-notes.md`.
2. Update `README.md` if the finding changes setup, build, flashing, or user expectations.
3. Update `docs/firmware-plan.md` if the finding changes architecture or feature sequencing.

Do not leave durable repo knowledge only in chat output.

## Repo-Specific Technical Notes

- The board uses a CO5300 display over QSPI and a CST9217 touch controller over I2C.
- Verified display and touch pin mappings are documented in `docs/repo-notes.md`.
- The current build needs `scripts/framework_workarounds.py` to add the active Arduino framework package's `libraries/Network/src` include path.
- Do not broaden that workaround to scan every installed Arduino framework package. That can mix incompatible `NetworkEvents` header versions and break linking.

## Current Implementation Status

- Display, LVGL, Wi-Fi, NTP time, and Open-Meteo weather fetch are scaffolded and building.
- Touch, RTC fallback, and final icon assets are still pending.