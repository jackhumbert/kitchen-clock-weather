#pragma once

#include <lvgl.h>

bool touch_init();
bool touch_ready();
void touch_read(lv_indev_t *indev, lv_indev_data_t *data);