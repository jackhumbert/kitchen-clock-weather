#pragma once

#include <stdint.h>

enum class WeatherIcon : uint8_t {
    Unknown,
    ClearDay,
    ClearNight,
    PartlyCloudyDay,
    PartlyCloudyNight,
    Cloudy,
    Fog,
    Rain,
    Snow,
    Thunderstorm,
};

struct WeatherSnapshot {
    bool valid = false;
    bool stale = true;
    bool isDay = true;
    int currentTemp = 0;
    int highTemp = 0;
    int lowTemp = 0;
    uint32_t ageSeconds = 0;
    WeatherIcon icon = WeatherIcon::Unknown;
};

void weather_service_begin();
void weather_service_update();
bool weather_service_fetch_now();
const WeatherSnapshot &weather_service_get_snapshot();
const char *weather_service_icon_text(WeatherIcon icon);