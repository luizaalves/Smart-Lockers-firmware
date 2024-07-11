#include "srv_mqtt.h"

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
