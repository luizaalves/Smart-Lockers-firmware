#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "drv_wifi.h"

/**
 * @brief 
 * 
 * @param handler 
 * @param event_id 
 */
void srv_wifi_set_callback(wifi_event_handler_t handler, wifi_event_et event_id);

/**
 * @brief 
 * 
 * @param config 
 * @return wifi_err_et 
 */
wifi_err_et srv_wifi_start(wifi_config_st config);

/**
 * @brief 
 * 
 * @return wifi_err_et 
 */
wifi_err_et srv_wifi_connect(void);

#ifdef __cplusplus
}
#endif
