#pragma once

#include <stddef.h>

// #define LV_MEM_CUSTOM_INCLUDE <esp_heap_caps.h>   /*Header for the dynamic memory function*/
// #define LV_MEM_CUSTOM_ALLOC(size) heap_caps_malloc(size, MALLOC_CAP_8BIT)
// #define LV_MEM_CUSTOM_FREE(ptr) heap_caps_free(ptr)
// #define LV_MEM_CUSTOM_REALLOC(ptr, size) heap_caps_realloc(ptr, size, MALLOC_CAP_8BIT)

#include <lvgl.h>
#include <src/libs/tiny_ttf/lv_tiny_ttf.h>

#define RAJDHANI_MEDIUM RAJDHANI_MEDIUM
#define RAJDHANI_REGULAR RAJDHANI_REGULAR
#define RAJDHANI_SEMIBOLD RAJDHANI_SEMIBOLD
#define RAJDHANI_BOLD RAJDHANI_BOLD
#define FIRACODE_REGULAR FIRACODE_REGULAR

lv_font_t * RAJDHANI_SEMIBOLD(int);
lv_font_t * RAJDHANI_REGULAR(int);
lv_font_t * RAJDHANI_MEDIUM(int);
lv_font_t * FIRACODE_REGULAR(int);

#define FONT_IMPL(name) \
std::map<int, lv_font_t*> name##_FONTS; \
lv_font_t * name(int size) { \
    auto element = name##_FONTS.find(size); \
    if (element == name##_FONTS.end()) { \
        name##_FONTS[size] = lv_tiny_ttf_create_data(name##_TTF, name##_TTF_SIZE, size); \
    } \
    return name##_FONTS[size]; \
}
