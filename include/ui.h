#pragma once

#include "clock_service.h"
#include "weather_service.h"

void ui_init();
void ui_set_time(const ClockSnapshot &snapshot);
void ui_set_weather(const WeatherSnapshot &snapshot);
void ui_set_status(const char *text);