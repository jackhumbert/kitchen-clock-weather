#include <Arduino.h>
#include <WiFi.h>

#include <cstdio>

#include "app_config.h"
#include "wifi_service.h"

namespace {
uint32_t sLastConnectAttemptMs = 0;
char sStatusText[48] = "WiFi idle";

void start_connect()
{
    if (!wifi_service_is_configured()) {
        snprintf(sStatusText, sizeof(sStatusText), "Add include/secrets.h");
        return;
    }

    WiFi.mode(WIFI_STA);
    WiFi.persistent(false);
    WiFi.setAutoReconnect(true);
    WiFi.begin(AppConfig::kWifiSsid, AppConfig::kWifiPassword);
    sLastConnectAttemptMs = millis();
    snprintf(sStatusText, sizeof(sStatusText), "Connecting WiFi");
}
}

void wifi_service_begin()
{
    start_connect();
}

void wifi_service_update()
{
    if (!wifi_service_is_configured()) {
        snprintf(sStatusText, sizeof(sStatusText), "Add include/secrets.h");
        return;
    }

    if (WiFi.status() == WL_CONNECTED) {
        const String ipAddress = WiFi.localIP().toString();
        snprintf(sStatusText, sizeof(sStatusText), "WiFi %s", ipAddress.c_str());
        return;
    }

    if (millis() - sLastConnectAttemptMs >= AppConfig::kWifiRetryIntervalMs) {
        start_connect();
    }
}

bool wifi_service_is_configured()
{
    return AppConfig::wifiConfigured();
}

bool wifi_service_is_connected()
{
    return wifi_service_is_configured() && WiFi.status() == WL_CONNECTED;
}

const char *wifi_service_status_text()
{
    return sStatusText;
}