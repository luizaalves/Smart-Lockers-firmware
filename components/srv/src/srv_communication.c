#include "srv_communication.h"
#include "esp_log.h"

static const char *TAG = "SRV_COMM";

static void handler_on_sta_got_ip(void *arg, const char* event_base, int32_t event_id, void *event_data);
static void mqtt_event_handler(void *handler_args, const char* base, int32_t event_id, void *event_data);

void srv_comm_init(void)
{
        wifi_config_st config = 
        {
        .ssid = APP_WIFI_SSID,
        .password = APP_WIFI_PASSWORD,
        .mode = APP_WIFI_MODE,
        .interface = APP_WIFI_INTERFACE,
        .interface_description = APP_WIFI_IF_DESC
    };
    
    if(srv_wifi_start(config) != DRV_WIFI_OK) return;
    srv_wifi_connect();
    srv_wifi_set_callback(handler_on_sta_got_ip, APP_EVENT_IP_STA);
    return;
}

static void handler_on_sta_got_ip(void *arg, const char* event_base, int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    ESP_LOGI(TAG, "Got IPv4 event: Interface \"%s\" address: " IPSTR, esp_netif_get_desc(event->esp_netif), IP2STR(&event->ip_info.ip));
    ESP_LOGI(TAG, "- IPv4 address: " IPSTR ",", IP2STR(&event->ip_info.ip));
    srv_mqtt_start("mqtt://192.168.15.7:1883");
    srv_mqtt_set_callback(mqtt_event_handler);
}

static void mqtt_event_handler(void *handler_args, const char* base, int32_t event_id, void *event_data) 
{
    esp_mqtt_event_handle_t event = event_data;
    //esp_mqtt_client_handle_t client = event->client;
    //int msg_id;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            //msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "teste", 6, 0,0);
            //ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
} 