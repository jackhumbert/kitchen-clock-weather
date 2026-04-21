#pragma once

#include <stdint.h>

#if __has_include("secrets.h")
#include "secrets.h"
#endif

#ifndef WIFI_SSID
#define WIFI_SSID ""
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD ""
#endif

#ifndef WEATHER_LATITUDE
#define WEATHER_LATITUDE 999.0f
#endif

#ifndef WEATHER_LONGITUDE
#define WEATHER_LONGITUDE 999.0f
#endif

#ifndef WEATHER_TIMEZONE_NAME
#define WEATHER_TIMEZONE_NAME "America/New_York"
#endif

#ifndef WEATHER_TZ_POSIX
#define WEATHER_TZ_POSIX "EST5EDT,M3.2.0/2,M11.1.0/2"
#endif

#ifndef WEATHER_USE_METRIC
#define WEATHER_USE_METRIC 0
#endif

#ifndef NTP_SERVER_1
#define NTP_SERVER_1 "pool.ntp.org"
#endif

#ifndef NTP_SERVER_2
#define NTP_SERVER_2 "time.nist.gov"
#endif

namespace AppConfig {
inline constexpr char kWifiSsid[] = WIFI_SSID;
inline constexpr char kWifiPassword[] = WIFI_PASSWORD;
inline constexpr float kWeatherLatitude = WEATHER_LATITUDE;
inline constexpr float kWeatherLongitude = WEATHER_LONGITUDE;
inline constexpr char kWeatherTimezoneName[] = WEATHER_TIMEZONE_NAME;
inline constexpr char kWeatherTimezonePosix[] = WEATHER_TZ_POSIX;
inline constexpr bool kUseMetric = WEATHER_USE_METRIC != 0;
inline constexpr char kNtpServer1[] = NTP_SERVER_1;
inline constexpr char kNtpServer2[] = NTP_SERVER_2;
inline constexpr uint32_t kWifiRetryIntervalMs = 10000;
inline constexpr uint32_t kWeatherRefreshIntervalMs = 15UL * 60UL * 1000UL;
inline constexpr uint32_t kClockRetryIntervalMs = 30000;
inline constexpr uint32_t kClockRefreshIntervalMs = 1000;
inline constexpr uint32_t kWeatherUiRefreshIntervalMs = 2000;
inline constexpr uint32_t kClockSyncIntervalMs = 6UL * 60UL * 60UL * 1000UL;

inline constexpr bool wifiConfigured()
{
    return sizeof(kWifiSsid) > 1 && sizeof(kWifiPassword) > 1;
}

inline constexpr bool locationConfigured()
{
    return kWeatherLatitude >= -90.0f && kWeatherLatitude <= 90.0f &&
           kWeatherLongitude >= -180.0f && kWeatherLongitude <= 180.0f;
}
}