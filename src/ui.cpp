#include <Arduino.h>
#include <lvgl.h>

#include <cstdio>

#include "app_config.h"
#include "ui.h"

namespace {
lv_obj_t *sTimeLabel = nullptr;
lv_obj_t *sMetaLabel = nullptr;
lv_obj_t *sWeatherLabel = nullptr;
lv_obj_t *sCurrentTempLabel = nullptr;
lv_obj_t *sRangeLabel = nullptr;
lv_obj_t *sStatusLabel = nullptr;
}

void ui_init()
{
    lv_obj_t *screen = lv_screen_active();
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_text_color(screen, lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    sTimeLabel = lv_label_create(screen);
    lv_obj_set_style_text_font(sTimeLabel, &lv_font_montserrat_48, 0);
    lv_label_set_text(sTimeLabel, "--:--");
    lv_obj_align(sTimeLabel, LV_ALIGN_TOP_MID, 0, 48);

    sMetaLabel = lv_label_create(screen);
    lv_obj_set_style_text_font(sMetaLabel, &lv_font_montserrat_20, 0);
    lv_label_set_text(sMetaLabel, "Waiting for time");
    lv_obj_align_to(sMetaLabel, sTimeLabel, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    sWeatherLabel = lv_label_create(screen);
    lv_obj_set_style_text_font(sWeatherLabel, &lv_font_montserrat_22, 0);
    lv_label_set_text(sWeatherLabel, "WEATHER");
    lv_obj_align(screen, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align(sWeatherLabel, LV_ALIGN_CENTER, 0, -10);

    sCurrentTempLabel = lv_label_create(screen);
    lv_obj_set_style_text_font(sCurrentTempLabel, &lv_font_montserrat_48, 0);
    lv_label_set_text(sCurrentTempLabel, "--");
    lv_obj_align_to(sCurrentTempLabel, sWeatherLabel, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    sRangeLabel = lv_label_create(screen);
    lv_obj_set_style_text_font(sRangeLabel, &lv_font_montserrat_20, 0);
    lv_label_set_text(sRangeLabel, "H --  L --");
    lv_obj_align_to(sRangeLabel, sCurrentTempLabel, LV_ALIGN_OUT_BOTTOM_MID, 0, 12);

    sStatusLabel = lv_label_create(screen);
    lv_obj_set_width(sStatusLabel, 320);
    lv_label_set_long_mode(sStatusLabel, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_style_text_font(sStatusLabel, &lv_font_montserrat_14, 0);
    lv_label_set_text(sStatusLabel, "Booting");
    lv_obj_align(sStatusLabel, LV_ALIGN_BOTTOM_MID, 0, -22);
}

void ui_set_time(const ClockSnapshot &snapshot)
{
    lv_label_set_text(sTimeLabel, snapshot.timeText);
    lv_label_set_text(sMetaLabel, snapshot.metaText);
}

void ui_set_weather(const WeatherSnapshot &snapshot)
{
    if (!snapshot.valid) {
        lv_label_set_text(sWeatherLabel, "WEATHER");
        lv_label_set_text(sCurrentTempLabel, "--");
        lv_label_set_text(sRangeLabel, "H --  L --");
        return;
    }

    lv_label_set_text(sWeatherLabel, weather_service_icon_text(snapshot.icon));

    char currentTemp[16] = {};
    char rangeText[24] = {};
    snprintf(currentTemp, sizeof(currentTemp), "%d%c", snapshot.currentTemp, AppConfig::kUseMetric ? 'C' : 'F');
    snprintf(rangeText, sizeof(rangeText), "H %d  L %d", snapshot.highTemp, snapshot.lowTemp);

    lv_label_set_text(sCurrentTempLabel, currentTemp);
    lv_label_set_text(sRangeLabel, rangeText);
}

void ui_set_status(const char *text)
{
    lv_label_set_text(sStatusLabel, text);
}