#pragma once

#include <stdint.h>
#include "mqtt_client.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    DRV_MQTT_OK = 0,
    DRV_MQTT_ERR_NULL,
    DRV_MQTT_ERR_START
} mqtt_err_et;

typedef struct
{
    char *topic;
    char *data; 
    int32_t event_id;
    int msg_id;
    //client
}mqtt_event_cb;


typedef void (*mqtt_event_handler_t)(void* event_handler_arg,
                                        const char* event_base,
                                        int32_t event_id,
                                        void* event_data);

/**
 * @brief MQTT initialization
 * 
 * @param uri – MQTT address
 * @return mqtt_err_et Return DRV_MQTT_ERR_NULL if client NULL or 
 *                            DRV_MQTT_OK if succeed
 */
mqtt_err_et drv_mqtt_init(const char *uri);

/**
 * @brief Starts MQTT
 * 
* @return mqtt_err_et Return DRV_MQTT_ERR_NULL if client NULL, 
*                            DRV_MQTT_ERR_START if start fails or 
*                            DRV_MQTT_OK if succeed
*/
mqtt_err_et drv_mqtt_start(void);

/**
 * @brief Register callback
 * 
 * @param handler – the handler function which gets called when the event is dispatched
 */
void drv_mqtt_set_callback(mqtt_event_handler_t handler);

esp_mqtt_client_handle_t drv_mqtt_get_client(void);

void drv_mqtt_stop(void);

int drv_mqtt_publish(char *topic, const char *data, int len);

int drv_mqtt_subscribe(char *topic, int qos);

//void drv_mqtt_disconnect(void);

//void drv_mqtt_cleanup(void);

#ifdef __cplusplus
}
#endif
