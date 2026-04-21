#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

#include <cmath>

#include "app_config.h"
#include "weather_service.h"
#include "wifi_service.h"

namespace {
WeatherSnapshot sSnapshot;
uint32_t sLastFetchAttemptMs = 0;
uint32_t sLastSuccessMs = 0;

WeatherIcon map_weather_icon(int weatherCode, bool isDay)
{
    switch (weatherCode) {
        case 0:
            return isDay ? WeatherIcon::ClearDay : WeatherIcon::ClearNight;
        case 1:
        case 2:
            return isDay ? WeatherIcon::PartlyCloudyDay : WeatherIcon::PartlyCloudyNight;
        case 3:
            return WeatherIcon::Cloudy;
        case 45:
        case 48:
            return WeatherIcon::Fog;
        case 51:
        case 53:
        case 55:
        case 56:
        case 57:
        case 61:
        case 63:
        case 65:
        case 66:
        case 67:
        case 80:
        case 81:
        case 82:
            return WeatherIcon::Rain;
        case 71:
        case 73:
        case 75:
        case 77:
        case 85:
        case 86:
            return WeatherIcon::Snow;
        case 95:
        case 96:
        case 99:
            return WeatherIcon::Thunderstorm;
        default:
            return WeatherIcon::Unknown;
    }
}

String build_weather_url()
{
    String timezone = AppConfig::kWeatherTimezoneName;
    timezone.replace("/", "%2F");

    String url = "https://api.open-meteo.com/v1/forecast?latitude=";
    url += String(AppConfig::kWeatherLatitude, 4);
    url += "&longitude=";
    url += String(AppConfig::kWeatherLongitude, 4);
    url += "&current=temperature_2m,weather_code,is_day";
    url += "&daily=temperature_2m_max,temperature_2m_min,weather_code";
    url += AppConfig::kUseMetric ? "&temperature_unit=celsius" : "&temperature_unit=fahrenheit";
    url += "&timezone=";
    url += timezone;
    url += "&forecast_days=1";
    return url;
}

int rounded_int(float value)
{
    return static_cast<int>(lroundf(value));
}

bool parse_response(Stream &responseStream)
{
    DynamicJsonDocument doc(4096);
    const auto error = deserializeJson(doc, responseStream);
    if (error) {
        return false;
    }

    const JsonVariant current = doc["current"];
    const JsonVariant daily = doc["daily"];
    if (current.isNull() || daily.isNull()) {
        return false;
    }

    sSnapshot.valid = true;
    sSnapshot.stale = false;
    sSnapshot.isDay = current["is_day"] | 1;
    sSnapshot.currentTemp = rounded_int(current["temperature_2m"] | 0.0f);
    sSnapshot.highTemp = rounded_int(daily["temperature_2m_max"][0] | 0.0f);
    sSnapshot.lowTemp = rounded_int(daily["temperature_2m_min"][0] | 0.0f);
    sSnapshot.icon = map_weather_icon(current["weather_code"] | 0, sSnapshot.isDay);
    sSnapshot.ageSeconds = 0;
    return true;
}
}

void weather_service_begin()
{
    sSnapshot = WeatherSnapshot {};
}

void weather_service_update()
{
    const uint32_t now = millis();

    if (sSnapshot.valid && sLastSuccessMs != 0) {
        sSnapshot.ageSeconds = (now - sLastSuccessMs) / 1000;
    }

    if (!AppConfig::locationConfigured()) {
        return;
    }

    if (!wifi_service_is_connected()) {
        if (sSnapshot.valid) {
            sSnapshot.stale = true;
        }
        return;
    }

    if (!sSnapshot.valid || now - sLastFetchAttemptMs >= AppConfig::kWeatherRefreshIntervalMs) {
        weather_service_fetch_now();
    }
}

bool weather_service_fetch_now()
{
    if (!AppConfig::locationConfigured() || !wifi_service_is_connected()) {
        return false;
    }

    sLastFetchAttemptMs = millis();

    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;
    if (!http.begin(client, build_weather_url())) {
        return false;
    }

    const int httpCode = http.GET();
    if (httpCode != HTTP_CODE_OK) {
        http.end();
        if (sSnapshot.valid) {
            sSnapshot.stale = true;
        }
        return false;
    }

    const bool parsed = parse_response(http.getStream());
    http.end();

    if (!parsed) {
        if (sSnapshot.valid) {
            sSnapshot.stale = true;
        }
        return false;
    }

    sLastSuccessMs = millis();
    return true;
}

const WeatherSnapshot &weather_service_get_snapshot()
{
    return sSnapshot;
}

const char *weather_service_icon_text(WeatherIcon icon)
{
    switch (icon) {
        case WeatherIcon::ClearDay:
            return "CLEAR";
        case WeatherIcon::ClearNight:
            return "CLEAR NIGHT";
        case WeatherIcon::PartlyCloudyDay:
            return "PARTLY CLOUDY";
        case WeatherIcon::PartlyCloudyNight:
            return "PARTLY CLOUDY N";
        case WeatherIcon::Cloudy:
            return "CLOUDY";
        case WeatherIcon::Fog:
            return "FOG";
        case WeatherIcon::Rain:
            return "RAIN";
        case WeatherIcon::Snow:
            return "SNOW";
        case WeatherIcon::Thunderstorm:
            return "THUNDER";
        case WeatherIcon::Unknown:
        default:
            return "WEATHER";
    }
}