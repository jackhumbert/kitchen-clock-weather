#include <Arduino.h>
#include <Wire.h>

#include <SensorPCF85063.hpp>

#include <ctime>
#include <sys/time.h>

#include "app_config.h"
#include "board.h"
#include "rtc_service.h"

namespace {
SensorPCF85063 sRtc;
bool sRtcAvailable = false;
bool sRtcHasValidTime = false;

constexpr int kMinimumRtcYear = 2024;
constexpr int kMaximumRtcYear = 2099;

bool rtc_tm_looks_valid(const tm &timeInfo)
{
    const int year = timeInfo.tm_year + 1900;
    return year >= kMinimumRtcYear && year <= kMaximumRtcYear &&
           timeInfo.tm_mon >= 0 && timeInfo.tm_mon <= 11 &&
           timeInfo.tm_mday >= 1 && timeInfo.tm_mday <= 31 &&
           timeInfo.tm_hour >= 0 && timeInfo.tm_hour <= 23 &&
           timeInfo.tm_min >= 0 && timeInfo.tm_min <= 59 &&
           timeInfo.tm_sec >= 0 && timeInfo.tm_sec <= 59;
}

bool read_rtc_time(tm &timeInfo)
{
    if (!sRtcAvailable || !sRtc.isClockIntegrityGuaranteed()) {
        return false;
    }

    timeInfo = {};
    sRtc.getDateTime(&timeInfo);
    timeInfo.tm_isdst = 0;
    return rtc_tm_looks_valid(timeInfo);
}

time_t tm_to_epoch_utc(const tm &utcTime)
{
    tm timeCopy = utcTime;
    timeCopy.tm_isdst = 0;

    setenv("TZ", "UTC0", 1);
    tzset();
    const time_t epoch = mktime(&timeCopy);
    setenv("TZ", AppConfig::kWeatherTimezonePosix, 1);
    tzset();

    return epoch;
}
}

bool rtc_service_begin()
{
    if (sRtcAvailable) {
        return true;
    }

    if (!sRtc.begin(Wire, BoardConfig::kI2cSda, BoardConfig::kI2cScl)) {
        sRtcAvailable = false;
        sRtcHasValidTime = false;
        return false;
    }

    sRtc.start();
    sRtcAvailable = true;

    tm timeInfo = {};
    sRtcHasValidTime = read_rtc_time(timeInfo);
    Serial.printf("rtc online: %s (%s)\n", rtc_service_chip_name(), sRtcHasValidTime ? "valid" : "invalid");
    return true;
}

bool rtc_service_is_available()
{
    return sRtcAvailable;
}

bool rtc_service_has_valid_time()
{
    return sRtcAvailable && sRtcHasValidTime;
}

bool rtc_service_sync_system_clock()
{
    if (!sRtcAvailable) {
        return false;
    }

    tm timeInfo = {};
    if (!read_rtc_time(timeInfo)) {
        sRtcHasValidTime = false;
        return false;
    }

    const time_t epoch = tm_to_epoch_utc(timeInfo);
    if (epoch <= 0) {
        sRtcHasValidTime = false;
        return false;
    }

    timeval timeValue = {};
    timeValue.tv_sec = epoch;
    timeValue.tv_usec = 0;
    if (settimeofday(&timeValue, nullptr) != 0) {
        return false;
    }

    sRtcHasValidTime = true;
    return true;
}

bool rtc_service_store_system_clock()
{
    if (!sRtcAvailable) {
        return false;
    }

    const time_t now = time(nullptr);
    if (now <= 0) {
        return false;
    }

    tm utcTime = {};
    if (gmtime_r(&now, &utcTime) == nullptr) {
        return false;
    }

    sRtc.setDateTime(utcTime);
    sRtcHasValidTime = true;
    return true;
}

const char *rtc_service_chip_name()
{
    return "PCF85063";
}