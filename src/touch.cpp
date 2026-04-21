#include <Arduino.h>
#include <Wire.h>

#include <TouchDrv.hpp>

#include "board.h"
#include "touch.h"

namespace {
TouchDrvCST92xx sTouch;
lv_point_t sLastPoint = {0, 0};
bool sTouchReady = false;

int16_t clamp_coord(int32_t value, int16_t maxValue)
{
    if (value < 0) {
        return 0;
    }

    if (value > maxValue) {
        return maxValue;
    }

    return static_cast<int16_t>(value);
}
}

bool touch_init()
{
    Wire.begin(BoardConfig::kI2cSda, BoardConfig::kI2cScl);

    sTouch.setPins(BoardConfig::kTouchReset, BoardConfig::kTouchInterrupt);
    if (!sTouch.begin(Wire, CST92XX_SLAVE_ADDRESS, BoardConfig::kI2cSda, BoardConfig::kI2cScl)) {
        sTouchReady = false;
        return false;
    }

    sTouch.setMaxCoordinates(BoardConfig::kDisplayWidth, BoardConfig::kDisplayHeight);
    sTouch.setMirrorXY(true, true);

    sTouchReady = true;

    Serial.printf("touch online: %s\n", sTouch.getModelName());
    return true;
}

bool touch_ready()
{
    return sTouchReady;
}

void touch_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    (void)indev;

    data->point = sLastPoint;
    data->state = LV_INDEV_STATE_RELEASED;

    if (!sTouchReady) {
        return;
    }

    const TouchPoints &touchPoints = sTouch.getTouchPoints();
    if (!touchPoints.hasPoints()) {
        return;
    }

    const TouchPoint &point = touchPoints.getPoint(0);
    sLastPoint.x = clamp_coord(point.x, BoardConfig::kDisplayWidth - 1);
    sLastPoint.y = clamp_coord(point.y, BoardConfig::kDisplayHeight - 1);

    data->point = sLastPoint;
    data->state = LV_INDEV_STATE_PRESSED;
}