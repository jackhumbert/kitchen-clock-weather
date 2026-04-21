#pragma once

#include <stdint.h>

struct ClockSnapshot {
    bool valid = false;
    char timeText[16] = "--:--";
    char metaText[32] = "Waiting for time";
};

void clock_service_begin();
bool clock_service_sync(uint32_t timeoutMs);
bool clock_service_has_valid_time();
bool clock_service_get_snapshot(ClockSnapshot &snapshot);