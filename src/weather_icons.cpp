#include <lvgl.h>

#include "weather_icons.h"

namespace {
constexpr int32_t kIconSize = 96;

LV_DRAW_BUF_DEFINE_STATIC(sWeatherIconDrawBuf, kIconSize, kIconSize, LV_COLOR_FORMAT_RGB565);

bool sDrawBufferReady = false;

lv_color_t cloud_color()
{
    return lv_color_hex(0xF5F9FF);
}

lv_color_t sun_color()
{
    return lv_color_hex(0xFFC857);
}

lv_color_t moon_color()
{
    return lv_color_hex(0xBDD3FF);
}

lv_color_t rain_color()
{
    return lv_color_hex(0x7FD7FF);
}

lv_color_t storm_color()
{
    return lv_color_hex(0xFFD166);
}

lv_color_t fog_color()
{
    return lv_color_hex(0xAEBACC);
}

void ensure_draw_buffer()
{
    if (!sDrawBufferReady) {
        LV_DRAW_BUF_INIT_STATIC(sWeatherIconDrawBuf);
        sDrawBufferReady = true;
    }
}

void draw_line(lv_layer_t *layer,
               int16_t x1,
               int16_t y1,
               int16_t x2,
               int16_t y2,
               lv_color_t color,
               int16_t width)
{
    lv_draw_line_dsc_t dsc;
    lv_draw_line_dsc_init(&dsc);
    dsc.color = color;
    dsc.width = width;
    dsc.round_start = 1;
    dsc.round_end = 1;
    dsc.p1.x = x1;
    dsc.p1.y = y1;
    dsc.p2.x = x2;
    dsc.p2.y = y2;
    lv_draw_line(layer, &dsc);
}

void draw_rect(lv_layer_t *layer,
               int16_t x1,
               int16_t y1,
               int16_t x2,
               int16_t y2,
               lv_color_t color,
               int16_t radius)
{
    lv_draw_rect_dsc_t dsc;
    lv_draw_rect_dsc_init(&dsc);
    dsc.bg_color = color;
    dsc.bg_opa = LV_OPA_COVER;
    dsc.border_width = 0;
    dsc.radius = radius;

    lv_area_t area = {x1, y1, x2, y2};
    lv_draw_rect(layer, &dsc, &area);
}

void draw_circle(lv_layer_t *layer, int16_t cx, int16_t cy, int16_t radius, lv_color_t color)
{
    draw_rect(layer, cx - radius, cy - radius, cx + radius, cy + radius, color, LV_RADIUS_CIRCLE);
}

void draw_cloud(lv_layer_t *layer, int16_t cx, int16_t cy)
{
    const lv_color_t color = cloud_color();
    draw_circle(layer, cx - 20, cy + 4, 14, color);
    draw_circle(layer, cx, cy - 6, 20, color);
    draw_circle(layer, cx + 22, cy + 4, 13, color);
    draw_rect(layer, cx - 34, cy + 4, cx + 34, cy + 26, color, 12);
}

void draw_sun(lv_layer_t *layer, int16_t cx, int16_t cy, int16_t radius, int16_t rayLength)
{
    const lv_color_t color = sun_color();
    draw_circle(layer, cx, cy, radius, color);

    draw_line(layer, cx, cy - radius - rayLength, cx, cy - radius - 4, color, 4);
    draw_line(layer, cx, cy + radius + 4, cx, cy + radius + rayLength, color, 4);
    draw_line(layer, cx - radius - rayLength, cy, cx - radius - 4, cy, color, 4);
    draw_line(layer, cx + radius + 4, cy, cx + radius + rayLength, cy, color, 4);
    draw_line(layer, cx - radius - 10, cy - radius - 10, cx - radius - 3, cy - radius - 3, color, 4);
    draw_line(layer, cx + radius + 3, cy - radius - 3, cx + radius + 10, cy - radius - 10, color, 4);
    draw_line(layer, cx - radius - 10, cy + radius + 10, cx - radius - 3, cy + radius + 3, color, 4);
    draw_line(layer, cx + radius + 3, cy + radius + 3, cx + radius + 10, cy + radius + 10, color, 4);
}

void draw_moon(lv_layer_t *layer, int16_t cx, int16_t cy, int16_t radius)
{
    draw_circle(layer, cx, cy, radius, moon_color());
    draw_circle(layer, cx + (radius / 2), cy - (radius / 3), radius - 3, lv_color_hex(0x000000));
}

void draw_clear_day(lv_layer_t *layer)
{
    draw_sun(layer, 48, 48, 18, 14);
}

void draw_clear_night(lv_layer_t *layer)
{
    draw_moon(layer, 48, 48, 22);
    draw_circle(layer, 27, 25, 3, moon_color());
    draw_circle(layer, 67, 21, 2, moon_color());
    draw_circle(layer, 70, 68, 3, moon_color());
}

void draw_partly_cloudy_day(lv_layer_t *layer)
{
    draw_sun(layer, 29, 28, 12, 12);
    draw_cloud(layer, 50, 52);
}

void draw_partly_cloudy_night(lv_layer_t *layer)
{
    draw_moon(layer, 32, 31, 16);
    draw_cloud(layer, 50, 52);
}

void draw_cloudy(lv_layer_t *layer)
{
    draw_cloud(layer, 48, 48);
}

void draw_fog(lv_layer_t *layer)
{
    draw_cloud(layer, 48, 38);
    draw_line(layer, 18, 66, 78, 66, fog_color(), 4);
    draw_line(layer, 22, 76, 82, 76, fog_color(), 4);
    draw_line(layer, 16, 86, 76, 86, fog_color(), 4);
}

void draw_rain(lv_layer_t *layer)
{
    draw_cloud(layer, 48, 36);
    draw_line(layer, 28, 66, 22, 84, rain_color(), 5);
    draw_line(layer, 48, 68, 42, 88, rain_color(), 5);
    draw_line(layer, 68, 66, 62, 84, rain_color(), 5);
}

void draw_snowflake(lv_layer_t *layer, int16_t cx, int16_t cy)
{
    draw_line(layer, cx - 6, cy, cx + 6, cy, rain_color(), 3);
    draw_line(layer, cx, cy - 6, cx, cy + 6, rain_color(), 3);
    draw_line(layer, cx - 5, cy - 5, cx + 5, cy + 5, rain_color(), 3);
    draw_line(layer, cx - 5, cy + 5, cx + 5, cy - 5, rain_color(), 3);
}

void draw_snow(lv_layer_t *layer)
{
    draw_cloud(layer, 48, 34);
    draw_snowflake(layer, 28, 74);
    draw_snowflake(layer, 48, 82);
    draw_snowflake(layer, 68, 74);
}

void draw_thunderstorm(lv_layer_t *layer)
{
    draw_cloud(layer, 48, 34);
    draw_line(layer, 46, 58, 34, 76, storm_color(), 6);
    draw_line(layer, 34, 76, 47, 76, storm_color(), 6);
    draw_line(layer, 47, 76, 38, 92, storm_color(), 6);
    draw_line(layer, 38, 92, 58, 70, storm_color(), 6);
    draw_line(layer, 68, 62, 62, 82, rain_color(), 5);
}
}

lv_obj_t *weather_icons_create(lv_obj_t *parent)
{
    ensure_draw_buffer();

    lv_obj_t *canvas = lv_canvas_create(parent);
    lv_canvas_set_draw_buf(canvas, &sWeatherIconDrawBuf);
    lv_obj_set_size(canvas, kIconSize, kIconSize);
    lv_obj_clear_flag(canvas, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_width(canvas, 0, 0);
    lv_obj_set_style_pad_all(canvas, 0, 0);

    weather_icons_set_icon(canvas, WeatherIcon::Unknown);
    return canvas;
}

void weather_icons_set_icon(lv_obj_t *canvas, WeatherIcon icon)
{
    lv_canvas_fill_bg(canvas, lv_color_hex(0x000000), LV_OPA_COVER);

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    switch (icon) {
        case WeatherIcon::ClearDay:
            draw_clear_day(&layer);
            break;
        case WeatherIcon::ClearNight:
            draw_clear_night(&layer);
            break;
        case WeatherIcon::PartlyCloudyDay:
            draw_partly_cloudy_day(&layer);
            break;
        case WeatherIcon::PartlyCloudyNight:
            draw_partly_cloudy_night(&layer);
            break;
        case WeatherIcon::Cloudy:
            draw_cloudy(&layer);
            break;
        case WeatherIcon::Fog:
            draw_fog(&layer);
            break;
        case WeatherIcon::Rain:
            draw_rain(&layer);
            break;
        case WeatherIcon::Snow:
            draw_snow(&layer);
            break;
        case WeatherIcon::Thunderstorm:
            draw_thunderstorm(&layer);
            break;
        case WeatherIcon::Unknown:
        default:
            draw_cloudy(&layer);
            break;
    }

    lv_canvas_finish_layer(canvas, &layer);
}