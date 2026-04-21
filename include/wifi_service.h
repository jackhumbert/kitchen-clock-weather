#pragma once

void wifi_service_begin();
void wifi_service_update();
bool wifi_service_is_configured();
bool wifi_service_is_connected();
const char *wifi_service_status_text();