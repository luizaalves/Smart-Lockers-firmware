#pragma once

#include "drv_gpio.h"
#define APP_BUTTON_GPIO 15
#define APP_WIFI_AP_SSID "LOCKER"
#define APP_WIFI_AP_PASS ""
#define APP_EVENT_IP_AP DRV_WIFI_IP_AP

void srv_button_init(void);
