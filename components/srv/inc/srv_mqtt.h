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

esp_mqtt_client_handle_t srv_mqtt_get_client(void);

void srv_mqtt_stop(void);

int srv_mqtt_subscribe(char *topic, int qos);

int srv_mqtt_publish(char *topic, const char *data, int len);

#ifdef __cplusplus
}
#endif
