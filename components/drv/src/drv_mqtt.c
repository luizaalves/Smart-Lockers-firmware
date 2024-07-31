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


esp_mqtt_client_handle_t drv_mqtt_get_client(void)
{
    return client;
}

void drv_mqtt_stop(void) 
{
    if (client != NULL) {
        ESP_LOGI(TAG, "Stopping MQTT client");

        // Para o cliente MQTT
        esp_err_t err = esp_mqtt_client_stop(client);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to stop MQTT client: %s", esp_err_to_name(err));
        }

        // Destrói o cliente MQTT para liberar recursos
        err = esp_mqtt_client_destroy(client);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to destroy MQTT client: %s", esp_err_to_name(err));
        } else {
            client = NULL; // Defina o ponteiro como NULL para evitar referências futuras
        }

        ESP_LOGI(TAG, "MQTT client stopped");
    } else {
        ESP_LOGW(TAG, "MQTT client is not initialized or already stopped");
    }
}

int drv_mqtt_subscribe(char *topic, int qos)
{
    return esp_mqtt_client_subscribe(client, topic, qos);

}
int drv_mqtt_publish(char *topic, const char *data, int len)
{
    return esp_mqtt_client_publish(client, topic, data, len, 2,0);

}