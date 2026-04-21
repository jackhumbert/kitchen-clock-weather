#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <lvgl.h>

#include "board.h"
#include "display.h"
#include "touch.h"

namespace {
Arduino_DataBus *sBus = nullptr;
Arduino_GFX *sGfx = nullptr;
lv_display_t *sLvDisplay = nullptr;
lv_indev_t *sLvTouchInput = nullptr;
uint8_t *sBuffer1 = nullptr;
uint8_t *sBuffer2 = nullptr;

void display_flush(lv_display_t *display, const lv_area_t *area, uint8_t *pxMap)
{
    const uint32_t width = area->x2 - area->x1 + 1;
    const uint32_t height = area->y2 - area->y1 + 1;

    sGfx->draw16bitRGBBitmap(
        area->x1,
        area->y1,
        reinterpret_cast<uint16_t *>(pxMap),
        width,
        height);

    lv_display_flush_ready(display);
}

uint32_t tick_millis()
{
    return millis();
}
}

bool display_init()
{
    sBus = new Arduino_ESP32QSPI(
        BoardConfig::kLcdCs,
        BoardConfig::kLcdSclk,
        BoardConfig::kLcdSdio0,
        BoardConfig::kLcdSdio1,
        BoardConfig::kLcdSdio2,
        BoardConfig::kLcdSdio3);

    sGfx = new Arduino_CO5300(
        sBus,
        BoardConfig::kLcdReset,
        0,
        BoardConfig::kDisplayWidth,
        BoardConfig::kDisplayHeight,
        6,
        0,
        0,
        0);

    if (!sGfx->begin()) {
        return false;
    }

    sGfx->fillScreen(0x0000);

    lv_init();
    lv_tick_set_cb(tick_millis);

    const uint32_t bufferBytes = BoardConfig::kDisplayWidth * BoardConfig::kLvglBufferLines * sizeof(uint16_t);
    sBuffer1 = static_cast<uint8_t *>(malloc(bufferBytes));
    sBuffer2 = static_cast<uint8_t *>(malloc(bufferBytes));
    if (sBuffer1 == nullptr || sBuffer2 == nullptr) {
        return false;
    }

    sLvDisplay = lv_display_create(BoardConfig::kDisplayWidth, BoardConfig::kDisplayHeight);
    lv_display_set_flush_cb(sLvDisplay, display_flush);
    lv_display_set_buffers(sLvDisplay, sBuffer1, sBuffer2, bufferBytes, LV_DISPLAY_RENDER_MODE_PARTIAL);

    if (touch_init()) {
        sLvTouchInput = lv_indev_create();
        lv_indev_set_type(sLvTouchInput, LV_INDEV_TYPE_POINTER);
        lv_indev_set_display(sLvTouchInput, sLvDisplay);
        lv_indev_set_read_cb(sLvTouchInput, touch_read);
    } else {
        Serial.println("touch_init failed; continuing without touch input");
    }

    return true;
}

void display_process()
{
    lv_timer_handler();
}

uint16_t display_width()
{
    return BoardConfig::kDisplayWidth;
}

uint16_t display_height()
{
    return BoardConfig::kDisplayHeight;
}