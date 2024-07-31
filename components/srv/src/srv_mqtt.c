#include "srv_mqtt.h"
#include <stdio.h>
#include "esp_log.h"

void srv_mqtt_set_callback(mqtt_event_handler_t handler)
{
    drv_mqtt_set_callback(handler);
}

mqtt_err_et srv_mqtt_start(const char *uri)
{
    drv_mqtt_init(uri);
    return drv_mqtt_start();
}
esp_mqtt_client_handle_t srv_mqtt_get_client(void)
{
    return drv_mqtt_get_client();
}

void srv_mqtt_stop(void)
{
    drv_mqtt_stop();
}

int srv_mqtt_subscribe(char *topic, int qos)
{
    return drv_mqtt_subscribe(topic, qos);

}
int srv_mqtt_publish(char *topic, const char *data, int len)
{
    return drv_mqtt_publish(topic, data, len);

}