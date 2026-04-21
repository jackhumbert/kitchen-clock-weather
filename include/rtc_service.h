#pragma once

bool rtc_service_begin();
bool rtc_service_is_available();
bool rtc_service_has_valid_time();
bool rtc_service_sync_system_clock();
bool rtc_service_store_system_clock();
const char *rtc_service_chip_name();