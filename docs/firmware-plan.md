# ESP32-S3-Touch-AMOLED-1.75-B Firmware Plan

## Goal

Build PlatformIO firmware for the Waveshare ESP32-S3-Touch-AMOLED-1.75-B that:

- Connects to home Wi-Fi.
- Displays local time with automatic EST/EDT handling.
- Displays current temperature, daily high, daily low, and a weather icon.
- Uses LVGL 9.x for the UI.
- Boots reliably even if Wi-Fi is temporarily unavailable.

## Recommendation

Use PlatformIO with the Arduino framework first, not ESP-IDF.

Reasoning:

- Waveshare already provides Arduino examples for this board with the correct CO5300 display and CST9217 touch wiring.
- Arduino_GFX already has LVGL 9 examples, which avoids porting old LVGL 8 glue code.
- For a weather clock, Arduino gives the shortest path to a stable result while still running on PlatformIO.

Do not start from Waveshare's LVGL 8 sketches as-is. Reuse only the board-specific pin definitions and device initialization patterns from their examples, then build the LVGL layer using LVGL 9 APIs.

## Board Facts

Confirmed hardware for the 1.75-B:

- Same electronics as the standard 1.75 board, but with the protective case.
- The intended board identifies itself during upload as 8 MB embedded PSRAM and 16 MB embedded flash.
- The earlier 2 MB / 4 MB hardware readings came from a different ESP32 that was attached at the same time, not from the target board.
- 466 x 466 round AMOLED.
- CO5300 display driver over QSPI.
- CST9217 touch controller over I2C.
- PCF85063 RTC.

Relevant board pins from Waveshare's Arduino examples:

- `LCD_SDIO0 = 4`
- `LCD_SDIO1 = 5`
- `LCD_SDIO2 = 6`
- `LCD_SDIO3 = 7`
- `LCD_SCLK = 38`
- `LCD_CS = 12`
- `LCD_RESET = 39`
- `IIC_SDA = 15`
- `IIC_SCL = 14`
- `TP_INT = 11`
- `TP_RESET = 40`

## Project Shape

Recommended layout:

```text
platformio.ini
src/
  main.cpp
  app.cpp
  board.cpp
  display.cpp
  touch.cpp
  clock_service.cpp
  weather_service.cpp
  wifi_service.cpp
  ui.cpp
  weather_icons.cpp
include/
  app_config.h
  secrets.example.h
  secrets.h              # local only, gitignored
  board.h
  display.h
  touch.h
  clock_service.h
  weather_service.h
  wifi_service.h
  ui.h
  weather_icons.h
lib/
  vendor/                # only if PlatformIO registry packages are incomplete
docs/
  firmware-plan.md
```

## Stack

Core stack:

- PlatformIO
- Arduino framework for ESP32-S3
- LVGL 9.x
- Arduino_GFX for the CO5300 display path
- Waveshare board pin config and touch/RTC support from their examples or vendored sources
- Built-in `WiFi.h`, `HTTPClient.h`, `time.h`, `Wire.h`
- `ArduinoJson` for parsing weather API responses

Likely dependency strategy:

- Prefer PlatformIO libraries where available.
- If Waveshare-specific support is awkward in the PlatformIO registry, vendor only the minimum required sources into `lib/vendor/`.
- Keep vendored code minimal: pin config, CST9217 touch driver, and PCF85063 RTC support if needed.

## Time Strategy

Use system time as the runtime source of truth and the RTC as the offline fallback.

Boot sequence:

1. Initialize display and show a simple "starting" screen.
2. Initialize RTC.
3. Read RTC and seed system time if it looks valid.
4. Connect Wi-Fi.
5. Sync NTP.
6. Set timezone for Eastern time with DST.
7. Write corrected time back to RTC after successful NTP sync.

Recommended timezone handling:

- Use `setenv("TZ", "EST5EDT,M3.2.0/2,M11.1.0/2", 1);`
- Then call `tzset();`
- Use `getLocalTime()` or `localtime_r()` for display formatting.

This gives automatic EST/EDT switching without hardcoding offsets in app logic.

## Weather Strategy

Use Open-Meteo directly from the device for the first version.

Why this is the best first step:

- No API key required for low-volume personal use.
- Can return current temperature, daily high/low, and WMO weather code in one request.
- Can return timestamps in local timezone.

Recommended request shape:

```text
https://api.open-meteo.com/v1/forecast?
latitude=<LAT>&longitude=<LON>&
current=temperature_2m,weather_code,is_day&
daily=temperature_2m_max,temperature_2m_min,weather_code&
temperature_unit=fahrenheit&
timezone=America/New_York&
forecast_days=1
```

Data we need from the response:

- `current.temperature_2m`
- `current.weather_code`
- `current.is_day`
- `daily.temperature_2m_max[0]`
- `daily.temperature_2m_min[0]`
- `daily.weather_code[0]`

## Weather Icon Plan

Use a small custom icon set mapped from WMO weather codes.

Recommended first-pass icon buckets:

- clear day
- clear night
- partly cloudy day
- partly cloudy night
- cloudy
- fog
- rain
- snow
- thunderstorm

Mapping logic:

- Use `weather_code` plus `is_day`.
- Collapse detailed WMO codes into the smaller icon set for simplicity.

Implementation choice:

- Pre-convert icons into LVGL image assets and compile them into flash.
- Avoid runtime PNG/JPEG decoding for v1.

## UI Plan

Design for the round 466 x 466 display, not a rectangular layout forced into a circle.

Safe layout zones:

- Keep primary content inside an approximate 380 to 400 px circle.
- Avoid small text near the outer edge.

Recommended layout:

- Top: current time, large and centered.
- Below time: timezone label such as `EST` or `EDT` and optional date.
- Center: weather icon.
- Below icon: current temperature, large.
- Bottom row: high and low temperatures.
- Small footer or corner indicator: Wi-Fi or sync status.

Example visual hierarchy:

```text
        8:42
       EDT Tue

        [icon]
         61 F

      H 67   L 49
```

LVGL guidance:

- Use one screen only for v1.
- Avoid complex animations.
- Update only labels and icon objects when data changes.
- Keep the main loop non-blocking.

## Burn-In Mitigation

This is an AMOLED panel, so a static clock UI should account for burn-in.

Minimum protections for v1:

- Reduce brightness at night.
- Slightly shift the main content by 1 to 2 px every few minutes.
- Dim after a period of no interaction if touch wake is desired later.

## State Model

Suggested app states:

- booting
- wifi_connecting
- time_syncing
- weather_loading
- ready
- degraded_offline
- error

Rules:

- If Wi-Fi is down but RTC time is valid, still show the clock and last cached weather.
- If weather fetch fails, keep the previous successful payload and mark it stale.
- If no RTC and no NTP are available, show a clear unsynced indicator instead of fake time.

## Update Cadence

Use non-blocking `millis()` scheduling.

Suggested intervals:

- LVGL handler: every 5 ms in `loop()`.
- Time label refresh: every 1 second.
- Wi-Fi reconnect check: every 5 to 10 seconds while disconnected.
- NTP resync: every 6 to 12 hours.
- RTC write-back: after each successful NTP sync.
- Weather refresh: every 10 to 15 minutes.

## Module Responsibilities

`board.cpp`

- Centralize pins and board setup.
- Own any Waveshare-specific init ordering.

`display.cpp`

- Initialize Arduino_GFX with CO5300 QSPI settings.
- Create LVGL display, buffers, and flush callback using LVGL 9 APIs.

`touch.cpp`

- Reset and initialize CST9217.
- Expose an LVGL 9 read callback.

`clock_service.cpp`

- Initialize timezone.
- Sync NTP.
- Read and write RTC.
- Expose formatted time/date strings.

`weather_service.cpp`

- Build API URL from fixed config.
- Perform HTTP GET.
- Parse JSON.
- Map WMO codes to internal icon enums.
- Cache last good payload.

`wifi_service.cpp`

- Connect to STA Wi-Fi.
- Expose link state.
- Retry with backoff.

`ui.cpp`

- Create objects once.
- Expose `ui_set_time(...)`, `ui_set_weather(...)`, `ui_set_status(...)`.
- Keep LVGL object references private to the module.

## Recommended Bring-Up Order

1. PlatformIO project boots and logs over USB serial.
2. Display lights and shows a static LVGL 9 label.
3. Touch input reports coordinates in serial.
4. Clock screen shows RTC time.
5. NTP sync works and EST/EDT formatting is correct.
6. Weather fetch works and values render on screen.
7. Weather icon mapping is complete.
8. Offline behavior is clean and understandable.
9. Burn-in mitigation and brightness control are added.

## Risks To Handle Early

1. Waveshare examples are centered around LVGL 8, so copying their display registration code directly will cause unnecessary churn.
2. PlatformIO library resolution for Waveshare's bundled helper libraries may be inconsistent, so plan for a minimal vendored `lib/vendor/` fallback.
3. Generic ESP32-S3 board settings in PlatformIO may need adjustment for USB CDC and PSRAM behavior.
4. Touch orientation may need one calibration pass using mirror and swap settings.
5. A static AMOLED UI can burn in if brightness and pixel drift are ignored.

## Open Decisions

These do not block v1, but should be decided before implementation starts:

1. Should Wi-Fi credentials be compiled in via `secrets.h`, or do we want runtime provisioning later?
2. Is the weather location fixed by latitude and longitude, or should it be configurable later?
3. Do we want Fahrenheit only, or a unit toggle?
4. Should the date be shown on the main screen, or time plus weather only?

## Acceptance Criteria For V1

- Cold boot reaches the main screen without crashing.
- Time displays correctly in Eastern time and flips between EST and EDT automatically.
- Weather data refreshes without blocking the UI.
- Current temp, high, low, and icon are visible and readable from across a room.
- If Wi-Fi is lost, time continues from RTC or system clock and the last weather payload remains visible with a stale indicator.

## Practical First Sprint

The first implementation sprint should only aim to prove the hardware and architecture:

1. Static LVGL 9 screen on the CO5300 display.
2. CST9217 touch callback wired into LVGL 9.
3. Wi-Fi connection status on screen.
4. NTP plus EST/EDT time display.
5. One successful Open-Meteo fetch rendered on screen.

Once that is stable, the rest is polish rather than bring-up.