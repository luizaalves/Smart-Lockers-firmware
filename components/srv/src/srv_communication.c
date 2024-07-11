#include "srv_communication.h"

#include "srv_mqtt.h"
#include "srv_wifi.h"
#include "srv_rfid.h"
#include "drv_nvs.h"
#include "esp_log.h"
#include "drv_nvs.h"
#include "srv_button.h"

static const char *TAG = "SRV_COMM";

static void handler_on_sta_got_ip(void *arg, const char* event_base, int32_t event_id, void *event_data);
static void mqtt_event_handler(void *handler_args, const char* base, int32_t event_id, void *event_data);
static void rfid_handler(void* arg, esp_event_base_t base, int32_t event_id, void* event_data);

void srv_comm_init(void)
{
    drv_nvs_init();
    srv_button_init();
    char *ssid=malloc(32);
    char *password=malloc(32);
    size_t len_ssid;
    size_t len_pass;

    drv_nvs_err_et err = drv_nvs_get("storage", WIFI_SSID_KEY, ssid, &len_ssid);
    if(err != DRV_NVS_OK) return;
    err = drv_nvs_get("storage", WIFI_PASS_KEY, password, &len_pass);
    if(err != DRV_NVS_OK) return;

    wifi_config_st config = 
    {
    .ssid = ssid,
    .password = password,
    .mode = APP_WIFI_MODE,
    .interface = APP_WIFI_INTERFACE,
    .interface_description = APP_WIFI_IF_DESC
    };

    rfid_config_st rfid_config = 
    {
        .host = VSPI_HOST,
        .miso = 25,
        .mosi = 23,
        .sck = 19,
        .sda = 22
    };
    
    if(srv_wifi_start(&config) != DRV_WIFI_OK) return;
    srv_wifi_connect();
    srv_wifi_set_callback(handler_on_sta_got_ip, APP_EVENT_IP_STA);
    srv_rfid_start(rfid_config);
    srv_rfid_set_callback(rfid_handler);
    free(ssid);
    free(password);
    return;
}

static void handler_on_sta_got_ip(void *arg, const char* event_base, int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    ESP_LOGI(TAG, "Got IPv4 event: Interface \"%s\" address: " IPSTR, esp_netif_get_desc(event->esp_netif), IP2STR(&event->ip_info.ip));
    ESP_LOGI(TAG, "- IPv4 address: " IPSTR ",", IP2STR(&event->ip_info.ip));
    srv_mqtt_start("mqtt://192.168.1.7:1883");
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

static void rfid_handler(void* arg, esp_event_base_t base, int32_t event_id, void* event_data)
{
    rc522_event_data_t* data = (rc522_event_data_t*) event_data;

    switch(event_id) {
        case RC522_EVENT_TAG_SCANNED: {
                rc522_tag_t* tag = (rc522_tag_t*) data->ptr;
                ESP_LOGI(TAG, "Tag scanned (sn: %" PRIu64 ")", tag->serial_number);
                //manda pro mqtt
                char str[21];
                snprintf(str, sizeof(str), "%llu", (unsigned long long)tag->serial_number); 
                int msg_id = esp_mqtt_client_publish(srv_mqtt_get_client(), "/door_command/request", str, sizeof(str), 0,0);
                ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d; value: %s", msg_id, str);
            }
            break;
    }
}
