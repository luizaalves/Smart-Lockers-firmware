#include <stdio.h>
#include "drv_mqtt.h"
#include "esp_log.h"

static const char* TAG = "DRV_MQTT";

static esp_mqtt_client_handle_t client = NULL;

mqtt_err_et drv_mqtt_init(const char *uri)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = uri,
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    if(client == NULL) 
    {
        ESP_LOGE(TAG, "esp_mqtt_client_init");
        return DRV_MQTT_ERR_NULL;
    }

    //drv_mqtt_set_callback(mqtt_event_handler);
    return DRV_MQTT_OK;
}

mqtt_err_et drv_mqtt_start(void)
{
    if(client == NULL)
    {
        ESP_LOGE(TAG, "client mqtt null.");
        return DRV_MQTT_ERR_NULL;
    }
    esp_err_t err = esp_mqtt_client_start(client);
    if(err != ESP_OK) 
    {
        ESP_LOGE(TAG, "esp_mqtt_client_start error: %d", err);
        return DRV_MQTT_ERR_START;
    }
    return DRV_MQTT_OK;
}

void drv_mqtt_set_callback(mqtt_event_handler_t handler) 
{
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, handler, client);
}

