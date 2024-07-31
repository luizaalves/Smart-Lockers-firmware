#pragma once

#ifdef __cplusplus
extern "C" {
#endif


#define APP_WIFI_MODE DRV_WIFI_MODE_STA
#define APP_WIFI_INTERFACE DRV_WIFI_IF_STA
#define APP_WIFI_IF_DESC "example"
#define APP_EVENT_IP_STA DRV_WIFI_IP_STA

typedef enum
{
    STATE_WAIT_DOOR_OPEN = 0,
    STATE_WAIT_DOOR_CLOSE = 1,
} state_door;

/**
 * @brief Init Wi-fi and MQTT communications 
 * 
 */
void srv_comm_init(void);
void srv_comm_config_wifi(void);

#ifdef __cplusplus
}
#endif
