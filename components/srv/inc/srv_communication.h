#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "srv_mqtt.h"
#include "srv_wifi.h"

#define APP_WIFI_SSID "VIVOFIBRA-8F61"
#define APP_WIFI_PASSWORD "PvKjTrq4yk"
#define APP_WIFI_MODE DRV_WIFI_MODE_STA
#define APP_WIFI_INTERFACE DRV_WIFI_IF_STA
#define APP_WIFI_IF_DESC "example"
#define APP_EVENT_IP_STA DRV_WIFI_IP_STA

/**
 * @brief Init Wi-fi and MQTT communications 
 * 
 */
void srv_comm_init(void);

#ifdef __cplusplus
}
#endif
