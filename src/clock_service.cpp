#include <Arduino.h>
#include <esp_sntp.h>

#include <cstring>
#include <ctime>

#include "app_config.h"
#include "clock_service.h"
#include "rtc_service.h"

namespace {
ClockSource sClockSource = ClockSource::None;
ClockSnapshot sLastValidSnapshot = {};
bool sHasLastValidSnapshot = false;

bool local_time_is_valid(uint32_t timeoutMs)
{
    struct tm timeInfo = {};
    return getLocalTime(&timeInfo, timeoutMs);
}

bool read_local_time(tm &timeInfo)
{
    const time_t now = time(nullptr);
    if (now <= 0) {
        return false;
    }

    if (localtime_r(&now, &timeInfo) == nullptr) {
        return false;
    }

    return timeInfo.tm_year > (2016 - 1900);
}

void cache_snapshot(const ClockSnapshot &snapshot)
{
    sLastValidSnapshot = snapshot;
    sHasLastValidSnapshot = true;
}
}

void clock_service_begin()
{
    setenv("TZ", AppConfig::kWeatherTimezonePosix, 1);
    tzset();

    sClockSource = ClockSource::None;
    sHasLastValidSnapshot = false;
    sLastValidSnapshot = {};
    rtc_service_begin();
    if (rtc_service_sync_system_clock()) {
        sClockSource = ClockSource::Rtc;
    }
}

bool clock_service_sync(uint32_t timeoutMs)
{
    configTzTime(AppConfig::kWeatherTimezonePosix, AppConfig::kNtpServer1, AppConfig::kNtpServer2);

    const uint32_t startMs = millis();
    while (millis() - startMs < timeoutMs) {
        if (local_time_is_valid(250)) {
            sClockSource = ClockSource::Ntp;
            rtc_service_store_system_clock();
            Serial.println("clock: NTP synced");
            return true;
        }
        delay(50);
    }

    Serial.println("clock: NTP sync timeout");

    return false;
}

bool clock_service_has_valid_time()
{
    tm timeInfo = {};
    return read_local_time(timeInfo) || sHasLastValidSnapshot;
}

bool clock_service_get_snapshot(ClockSnapshot &snapshot)
{
    struct tm timeInfo = {};
    if (!read_local_time(timeInfo)) {
        if (sHasLastValidSnapshot) {
            snapshot = sLastValidSnapshot;
            return true;
        }

        snapshot.valid = false;
        snprintf(snapshot.timeText, sizeof(snapshot.timeText), "--:--");
        snprintf(snapshot.metaText, sizeof(snapshot.metaText), "Waiting for NTP");
        return false;
    }

    snapshot.valid = true;
    strftime(snapshot.timeText, sizeof(snapshot.timeText), "%I:%M", &timeInfo);
    if (snapshot.timeText[0] == '0') {
        memmove(snapshot.timeText, snapshot.timeText + 1, strlen(snapshot.timeText));
    }

    char zoneText[8] = {};
    char dateText[24] = {};
    strftime(zoneText, sizeof(zoneText), "%Z", &timeInfo);
    strftime(dateText, sizeof(dateText), "%a %b %d", &timeInfo);
    snprintf(snapshot.metaText, sizeof(snapshot.metaText), "%s %s", zoneText, dateText);
    cache_snapshot(snapshot);
    return true;
}

ClockSource clock_service_source()
{
    return sClockSource;
}

bool clock_service_rtc_available()
{
    return rtc_service_is_available();
}