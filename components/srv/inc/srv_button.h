#pragma once

#include "drv_gpio.h"
#define APP_BUTTON_GPIO 15
#define APP_COMPARTMENT_1 32
#define APP_COMPARTMENT_2 33
#define APP_SENSOR_1 34
#define APP_SENSOR_2 35
#define APP_WIFI_AP_SSID "LOCKER"
#define APP_WIFI_AP_PASS ""
#define APP_EVENT_IP_AP DRV_WIFI_IP_AP

void srv_button_init(void);
