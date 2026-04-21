#pragma once

#include <lvgl.h>

#include "weather_service.h"

lv_obj_t *weather_icons_create(lv_obj_t *parent);
void weather_icons_set_icon(lv_obj_t *canvas, WeatherIcon icon);