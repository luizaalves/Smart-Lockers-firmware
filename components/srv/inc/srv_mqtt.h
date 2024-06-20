#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "drv_mqtt.h"

/**
 * @brief Register callback
 * 
 * @param handler – the handler function which gets called when the event is dispatched
 */
void srv_mqtt_set_callback(mqtt_event_handler_t handler);

/**
 * @brief 
 * 
 * @param uri 
 * @return mqtt_err_et 
 */
mqtt_err_et srv_mqtt_start(const char *uri);

#ifdef __cplusplus
}
#endif
