#include <Arduino.h>
#include <lvgl.h>

#include <cstdio>

#include "app_config.h"
#include "font.h"
#include "ui.h"
#include "weather_icons.h"

namespace {
lv_obj_t *sTimeLabel = nullptr;
lv_obj_t *sMetaLabel = nullptr;
lv_obj_t *sWeatherRow = nullptr;
lv_obj_t *sWeatherIcon = nullptr;
lv_obj_t *sWeatherLabel = nullptr;
lv_obj_t *sCurrentTempLabel = nullptr;
lv_obj_t *sRangeLabel = nullptr;
lv_obj_t *sStatusLabel = nullptr;
char sLastTimeText[16] = {};
char sLastMetaText[32] = {};
char sLastWeatherText[24] = {};
char sLastCurrentTempText[16] = {};
char sLastRangeText[24] = {};
WeatherIcon sLastWeatherIcon = WeatherIcon::Unknown;
bool sWeatherDisplayInitialized = false;

const lv_font_t *rajdhani_regular_font(int32_t size, const lv_font_t *fallback)
{
#if LV_USE_TINY_TTF
    lv_font_t *font = RAJDHANI_REGULAR(size);
    if (font != nullptr) {
        return font;
    }
#endif

    return fallback;
}

const lv_font_t *rajdhani_medium_font(int32_t size, const lv_font_t *fallback)
{
#if LV_USE_TINY_TTF
    lv_font_t *font = RAJDHANI_MEDIUM(size);
    if (font != nullptr) {
        return font;
    }
#endif

    return fallback;
}

const lv_font_t *rajdhani_semibold_font(int32_t size, const lv_font_t *fallback)
{
#if LV_USE_TINY_TTF
    lv_font_t *font = RAJDHANI_SEMIBOLD(size);
    if (font != nullptr) {
        return font;
    }
#endif

    return fallback;
}
}

void ui_init()
{
    lv_obj_t *screen = lv_screen_active();
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_text_color(screen, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(screen, LV_SCROLLBAR_MODE_OFF);

    sTimeLabel = lv_label_create(screen);
    lv_obj_set_width(sTimeLabel, 430);
    lv_obj_set_style_text_align(sTimeLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(sTimeLabel, rajdhani_regular_font(192, &lv_font_montserrat_48), 0);
    lv_obj_set_style_text_letter_space(sTimeLabel, -2, 0);
    lv_label_set_text(sTimeLabel, "--:--");
    lv_obj_align(sTimeLabel, LV_ALIGN_TOP_MID, 0, -8);

    sMetaLabel = lv_label_create(screen);
    lv_obj_set_width(sMetaLabel, 360);
    lv_obj_set_style_text_align(sMetaLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(sMetaLabel, rajdhani_medium_font(40, &lv_font_montserrat_20), 0);
    lv_obj_set_style_text_letter_space(sMetaLabel, 1, 0);
    lv_label_set_text(sMetaLabel, "Waiting for time");
    lv_obj_align_to(sMetaLabel, sTimeLabel, LV_ALIGN_OUT_BOTTOM_MID, 0, -10);

    sWeatherRow = lv_obj_create(screen);
    lv_obj_remove_style_all(sWeatherRow);
    lv_obj_set_size(sWeatherRow, 360, 118);
    lv_obj_set_layout(sWeatherRow, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(sWeatherRow, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(sWeatherRow, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(sWeatherRow, 14, 0);
    lv_obj_set_style_pad_all(sWeatherRow, 0, 0);
    lv_obj_clear_flag(sWeatherRow, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(sWeatherRow, LV_SCROLLBAR_MODE_OFF);
    lv_obj_align_to(sWeatherRow, sMetaLabel, LV_ALIGN_OUT_BOTTOM_MID, 0, -4);

    sWeatherIcon = weather_icons_create(sWeatherRow);

    sWeatherLabel = lv_label_create(screen);
    lv_obj_set_width(sWeatherLabel, 360);
    lv_obj_set_style_text_align(sWeatherLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(sWeatherLabel, rajdhani_semibold_font(28, &lv_font_montserrat_28), 0);
    lv_obj_set_style_text_letter_space(sWeatherLabel, 1, 0);
    lv_label_set_text(sWeatherLabel, "WEATHER");
    lv_obj_align_to(sWeatherLabel, sWeatherRow, LV_ALIGN_OUT_BOTTOM_MID, 0, -8);

    sCurrentTempLabel = lv_label_create(sWeatherRow);
    lv_obj_set_width(sCurrentTempLabel, 190);
    lv_obj_set_style_text_align(sCurrentTempLabel, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_text_font(sCurrentTempLabel, rajdhani_semibold_font(96, &lv_font_montserrat_48), 0);
    lv_obj_set_style_text_letter_space(sCurrentTempLabel, -1, 0);
    lv_label_set_text(sCurrentTempLabel, "--");

    sRangeLabel = lv_label_create(screen);
    lv_obj_set_width(sRangeLabel, 360);
    lv_obj_set_style_text_align(sRangeLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(sRangeLabel, rajdhani_medium_font(40, &lv_font_montserrat_20), 0);
    lv_obj_set_style_text_letter_space(sRangeLabel, 1, 0);
    lv_label_set_text(sRangeLabel, "H --  L --");
    lv_obj_align_to(sRangeLabel, sWeatherLabel, LV_ALIGN_OUT_BOTTOM_MID, 0, -10);

    sStatusLabel = lv_label_create(screen);
    lv_obj_set_width(sStatusLabel, 400);
    lv_label_set_long_mode(sStatusLabel, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_style_text_font(sStatusLabel, rajdhani_medium_font(28, &lv_font_montserrat_28), 0);
    lv_obj_set_style_text_letter_space(sStatusLabel, 1, 0);
    lv_label_set_text(sStatusLabel, "Booting");
    lv_obj_align(sStatusLabel, LV_ALIGN_BOTTOM_MID, 0, -10);
}

void ui_set_time(const ClockSnapshot &snapshot)
{
    if (strcmp(sLastTimeText, snapshot.timeText) != 0) {
        snprintf(sLastTimeText, sizeof(sLastTimeText), "%s", snapshot.timeText);
        lv_label_set_text(sTimeLabel, sLastTimeText);
    }

    if (strcmp(sLastMetaText, snapshot.metaText) != 0) {
        snprintf(sLastMetaText, sizeof(sLastMetaText), "%s", snapshot.metaText);
        lv_label_set_text(sMetaLabel, sLastMetaText);
    }
}

void ui_set_weather(const WeatherSnapshot &snapshot)
{
    WeatherIcon icon = WeatherIcon::Unknown;
    char weatherText[24] = {};
    char currentTemp[16] = {};
    char rangeText[24] = {};

    if (!snapshot.valid) {
        snprintf(weatherText, sizeof(weatherText), "%s", weather_service_status_text());
        snprintf(currentTemp, sizeof(currentTemp), "--");
        snprintf(rangeText, sizeof(rangeText), "H --  L --");
    } else {
        icon = snapshot.icon;
        snprintf(weatherText, sizeof(weatherText), "%s", weather_service_icon_text(snapshot.icon));
        snprintf(currentTemp, sizeof(currentTemp), "%d%c", snapshot.currentTemp, AppConfig::kUseMetric ? 'C' : 'F');
        snprintf(rangeText, sizeof(rangeText), "H %d  L %d", snapshot.highTemp, snapshot.lowTemp);
    }

    if (!sWeatherDisplayInitialized || sLastWeatherIcon != icon) {
        weather_icons_set_icon(sWeatherIcon, icon);
        sLastWeatherIcon = icon;
    }

    if (!sWeatherDisplayInitialized || strcmp(sLastWeatherText, weatherText) != 0) {
        snprintf(sLastWeatherText, sizeof(sLastWeatherText), "%s", weatherText);
        lv_label_set_text(sWeatherLabel, sLastWeatherText);
    }

    if (!sWeatherDisplayInitialized || strcmp(sLastCurrentTempText, currentTemp) != 0) {
        snprintf(sLastCurrentTempText, sizeof(sLastCurrentTempText), "%s", currentTemp);
        lv_label_set_text(sCurrentTempLabel, sLastCurrentTempText);
    }

    if (!sWeatherDisplayInitialized || strcmp(sLastRangeText, rangeText) != 0) {
        snprintf(sLastRangeText, sizeof(sLastRangeText), "%s", rangeText);
        lv_label_set_text(sRangeLabel, sLastRangeText);
    }

    sWeatherDisplayInitialized = true;
}

void ui_set_status(const char *text)
{
    lv_label_set_text(sStatusLabel, text);
}