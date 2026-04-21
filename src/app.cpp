#include <Arduino.h>

#include "app.h"
#include "app_config.h"
#include "clock_service.h"
#include "display.h"
#include "ui.h"
#include "weather_service.h"
#include "wifi_service.h"

namespace {
uint32_t sLastClockRefreshMs = 0;
uint32_t sLastWeatherRefreshMs = 0;
uint32_t sLastClockSyncAttemptMs = 0;
char sStatusText[48] = "Booting";

void update_status_text()
{
    const WeatherSnapshot &weather = weather_service_get_snapshot();
    const ClockSource clockSource = clock_service_source();

    if (!wifi_service_is_configured()) {
        snprintf(sStatusText, sizeof(sStatusText), "Add include/secrets.h");
    } else if (!clock_service_has_valid_time()) {
        snprintf(sStatusText, sizeof(sStatusText), "Waiting for time");
    } else if (!wifi_service_is_connected() && clockSource == ClockSource::Rtc) {
        snprintf(sStatusText, sizeof(sStatusText), "RTC fallback");
    } else if (!wifi_service_is_connected()) {
        snprintf(sStatusText, sizeof(sStatusText), "%s", wifi_service_status_text());
    } else if (!AppConfig::locationConfigured()) {
        snprintf(sStatusText, sizeof(sStatusText), "Set weather location");
    } else if (!weather.valid) {
        snprintf(sStatusText, sizeof(sStatusText), "Loading weather");
    } else if (weather.valid && weather.stale) {
        snprintf(sStatusText, sizeof(sStatusText), "Weather cached");
    } else if (clockSource == ClockSource::Rtc) {
        snprintf(sStatusText, sizeof(sStatusText), "Using RTC time");
    } else {
        snprintf(sStatusText, sizeof(sStatusText), "Ready");
    }

    ui_set_status(sStatusText);
}
}

void app_setup()
{
    Serial.begin(115200);
    delay(200);
    Serial.println();
    Serial.println("kitchen-clock-weather boot");

    if (!display_init()) {
        Serial.println("display_init failed");
        while (true) {
            delay(1000);
        }
    }

    ui_init();
    clock_service_begin();
    wifi_service_begin();
    weather_service_begin();

    ClockSnapshot clockSnapshot;
    clock_service_get_snapshot(clockSnapshot);
    ui_set_time(clockSnapshot);
    ui_set_weather(weather_service_get_snapshot());
    update_status_text();
}

void app_loop()
{
    const uint32_t now = millis();

    display_process();
    wifi_service_update();

    const uint32_t clockSyncIntervalMs = clock_service_source() == ClockSource::Ntp
                                             ? AppConfig::kClockSyncIntervalMs
                                             : AppConfig::kClockRetryIntervalMs;

    if (wifi_service_is_connected() &&
        (sLastClockSyncAttemptMs == 0 || now - sLastClockSyncAttemptMs >= clockSyncIntervalMs)) {
        clock_service_sync(5000);
        sLastClockSyncAttemptMs = now;
    }

    weather_service_update();

    if (now - sLastClockRefreshMs >= AppConfig::kClockRefreshIntervalMs) {
        ClockSnapshot clockSnapshot;
        clock_service_get_snapshot(clockSnapshot);
        ui_set_time(clockSnapshot);
        sLastClockRefreshMs = now;
    }

    if (now - sLastWeatherRefreshMs >= AppConfig::kWeatherUiRefreshIntervalMs) {
        ui_set_weather(weather_service_get_snapshot());
        sLastWeatherRefreshMs = now;
    }

    update_status_text();
}