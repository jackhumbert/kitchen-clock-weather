#include <Arduino.h>
#include <esp_sntp.h>

#include <cstring>
#include <ctime>

#include "app_config.h"
#include "clock_service.h"
#include "rtc_service.h"

namespace {
ClockSource sClockSource = ClockSource::None;
}

void clock_service_begin()
{
    setenv("TZ", AppConfig::kWeatherTimezonePosix, 1);
    tzset();

    sClockSource = ClockSource::None;
    rtc_service_begin();
    if (rtc_service_sync_system_clock()) {
        sClockSource = ClockSource::Rtc;
    }
}

bool clock_service_sync(uint32_t timeoutMs)
{
    configTzTime(AppConfig::kWeatherTimezonePosix, AppConfig::kNtpServer1, AppConfig::kNtpServer2);

    const uint32_t startMs = millis();
    struct tm timeInfo = {};
    while (millis() - startMs < timeoutMs) {
        if (sntp_get_sync_status() == SNTP_SYNC_STATUS_COMPLETED && getLocalTime(&timeInfo, 250)) {
            sClockSource = ClockSource::Ntp;
            rtc_service_store_system_clock();
            return true;
        }
        delay(50);
    }

    return false;
}

bool clock_service_has_valid_time()
{
    struct tm timeInfo = {};
    return getLocalTime(&timeInfo, 10);
}

bool clock_service_get_snapshot(ClockSnapshot &snapshot)
{
    struct tm timeInfo = {};
    if (!getLocalTime(&timeInfo, 10)) {
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