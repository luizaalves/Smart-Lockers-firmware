#include "srv_communication.h"

#include "srv_mqtt.h"
#include "srv_wifi.h"
#include "srv_rfid.h"
#include "drv_nvs.h"
#include "esp_log.h"
#include "drv_nvs.h"
#include "srv_button.h"
#include <time.h>
#include <stdio.h>
#include "esp_sntp.h"
#include "esp_netif_sntp.h"
#include "srv_hmi.h"

static const char *TAG = "SRV_COMM";

static TaskHandle_t door_handle=NULL;
hmi_cb led_cb = handler_hmi;
rc522_tag_t *tag = {0};

static void handler_on_sta_got_ip(void *arg, const char* event_base, int32_t event_id, void *event_data);
static void mqtt_event_handler(void *handler_args, const char* base, int32_t event_id, void *event_data);
static void rfid_handler(void* arg, esp_event_base_t base, int32_t event_id, void* event_data);
static void open_door(void *arg);
static uint8_t check_num_from_gpio(uint8_t num);
static uint8_t check_num_from_db(uint8_t num);
static uint8_t check_sensor_from_num(uint8_t num);
static void split_data(const char* data, int* num, char* locker_name, char *tag_rfid) ;

void srv_comm_init(void)
{
    drv_nvs_init();
    srv_button_init();
    srv_hmi_init();
    char *ssid=malloc(32);
    char *password=malloc(32);
    size_t len_ssid;
    size_t len_pass;

    drv_nvs_err_et err = drv_nvs_get_str("storage", WIFI_SSID_KEY, ssid, &len_ssid);
    if(err != DRV_NVS_OK) return;
    err = drv_nvs_get_str("storage", WIFI_PASS_KEY, password, &len_pass);
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
        .miso = 19,
        .mosi = 23,
        .sck = 18,
        .sda = 21
    };
    
    if(srv_wifi_start(&config) != DRV_WIFI_OK) return;
    srv_wifi_connect();
    srv_wifi_set_callback(handler_on_sta_got_ip, APP_EVENT_IP_STA);
    srv_rfid_start(rfid_config);
    srv_rfid_set_callback(rfid_handler);
    free(ssid);
    free(password);

    drv_gpio_set_direction(32, GPIO_MODE_OUTPUT);
    drv_gpio_set_direction(33, GPIO_MODE_OUTPUT);

    drv_gpio_set_direction(39, GPIO_MODE_INPUT);
    // drv_gpio_set_pull(39);
    drv_gpio_set_direction(36, GPIO_MODE_INPUT);
    // drv_gpio_set_pull(36);
    led_cb(SRV_LED_FREE_TO_USE,0);
    led_cb(SRV_LED_NO_FREE_DOORS,1);
    return;
}

static void handler_on_sta_got_ip(void *arg, const char* event_base, int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    ESP_LOGI(TAG, "Got IPv4 event: Interface \"%s\" address: " IPSTR, esp_netif_get_desc(event->esp_netif), IP2STR(&event->ip_info.ip));
    ESP_LOGI(TAG, "- IPv4 address: " IPSTR ",", IP2STR(&event->ip_info.ip));
    srv_mqtt_start("mqtt://192.168.1.7:1883");
    srv_mqtt_set_callback(mqtt_event_handler);
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
    esp_netif_sntp_init(&config);
}

static void mqtt_event_handler(void *handler_args, const char* base, int32_t event_id, void *event_data) 
{
    esp_mqtt_event_handle_t event = event_data;
    //esp_mqtt_client_handle_t client = event->client;
    //int msg_id;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            int response = srv_mqtt_subscribe("/door_command/response", 2);
            //liga led "ja da pra usar"
            led_cb(SRV_LED_FREE_TO_USE,1);
            led_cb(SRV_LED_NO_FREE_DOORS,0);

            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            //liga led "nao da pra usar!!!"
            led_cb(SRV_LED_FREE_TO_USE,0);
            led_cb(SRV_LED_NO_FREE_DOORS,1);

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
            if(strcmp(event->topic,"/door_command/response")==0)
            {
                int compartment = 0;
                char locker_name_received[20];
                char tag_received[20]="";
                char locker_name[20] = "";
                size_t locker_name_len = sizeof(locker_name);
                
                drv_nvs_get_str("storage", LOCKER_NAME_KEY, locker_name, &locker_name_len);

                split_data(event->data, &compartment, locker_name_received, &tag_received);
                ESP_LOGI(TAG, "NUM: %d", compartment);

                if(compartment == 0)
                {
                    // led_cb(SRV_LED_STORE_OBJ,0);
                    led_cb(SRV_LED_TAG_NOT_FOUND_IN_DB,1);
                    vTaskDelay(pdMS_TO_TICKS(2000));
                    led_cb(SRV_LED_TAG_NOT_FOUND_IN_DB,0);
                    led_cb(SRV_LED_FREE_TO_USE,1);
                    led_cb(SRV_LED_WAITING_ANSWER,0);
                    return;
                }
                else if(compartment == -1)
                {
                    led_cb(SRV_LED_FREE_TO_USE,0);
                    led_cb(SRV_LED_WAITING_ANSWER,0);
                    led_cb(SRV_LED_NO_FREE_DOORS,1);
                    led_cb(SRV_LED_STORE_OBJ,0);

                    
                    return;
                }
                char str[20];
                sprintf(str, "%llu", tag->serial_number);
                tag_received[strlen(str)] = '\0';
                if((strcmp(tag_received,str)!=0)||(strcmp(locker_name_received, locker_name)!=0)) return;
                compartment = check_num_from_db(compartment);
                eTaskState state_task = door_handle != NULL ? eTaskGetState(door_handle): eInvalid;
                if(state_task!=eRunning) xTaskCreate(open_door, "open_door", 1024*5, (void *)compartment, 10, &door_handle);

            }
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
            //liga led "tao usando, pera ai"
                tag = (rc522_tag_t*) data->ptr;
                ESP_LOGI(TAG, "Tag scanned (sn: %" PRIu64 ")", tag->serial_number);
                //manda pro mqtt
                char str[20];
                uint8_t num_compartments = 0;
                char locker_name[10] = "";
                size_t locker_name_len = sizeof(locker_name);
                drv_nvs_get_str("storage", LOCKER_NAME_KEY, locker_name, &locker_name_len);
                drv_nvs_get_u8("storage", LOCKER_NUM_KEY, &num_compartments);
                
                snprintf(str, sizeof(str), "%llu:%s:%u",(unsigned long long)tag->serial_number, locker_name, num_compartments); 
                int msg_id = srv_mqtt_publish("/door_command/request", str, strlen(str));
                led_cb(SRV_LED_FREE_TO_USE,0);
                led_cb(SRV_LED_NO_FREE_DOORS,0);
                led_cb(SRV_LED_WAITING_ANSWER,1);
                ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d; value: %s", msg_id, str);
                vTaskDelay(pdMS_TO_TICKS(1000));
            }
            break;
    }
}

static uint8_t check_num_from_db(uint8_t num)
{
    if(num == 1)
        return APP_COMPARTMENT_1;
    if(num == 2)
        return APP_COMPARTMENT_2;
    return 0;
}

static uint8_t check_sensor_from_num(uint8_t num)
{
    if(num == APP_COMPARTMENT_1)
        return APP_SENSOR_1;
    if(num == APP_COMPARTMENT_2)
        return APP_SENSOR_2;
    return 0;
}

static uint8_t check_num_from_gpio(uint8_t num)
{
    if(num == APP_COMPARTMENT_1)
        return 1;
    if(num == APP_COMPARTMENT_2)
        return 2;
    return 0;
}


static void open_door(void *arg)
{
    uint8_t compartment = arg;
    uint8_t sensor = check_sensor_from_num(compartment);
    state_door state = STATE_WAIT_DOOR_OPEN;
    time_t timestamp = 0;
    time_t timestamp2 = 0;
    //verifica de qual gpio esse compartimento esta associado 15
    //destrava tal gpio na condição imã fechado
    drv_gpio_set_level(compartment, 1);
    led_cb(SRV_LED_TAG_NOT_FOUND_IN_DB,0);

    for(;;)
    {
        if(state == STATE_WAIT_DOOR_OPEN)
        {
            if(drv_gpio_get_level(sensor)==0)
            {
                ESP_LOGI(TAG, "> afastou o imã");

                time(&timestamp);
                
                state = STATE_WAIT_DOOR_CLOSE;
            }
            vTaskDelay(pdMS_TO_TICKS(100));

            //quando detectar que o ima afastou, começa a contar o tempo

        }
        else if(state == STATE_WAIT_DOOR_CLOSE)
        {
            if(drv_gpio_get_level(sensor)==1)
            {
                time(&timestamp2);
                
                drv_gpio_set_level(compartment, 0);
                char locker_name[10] = "";
                size_t locker_name_len = sizeof(locker_name);
                drv_nvs_get_str("storage", LOCKER_NAME_KEY, locker_name, &locker_name_len);

                char str[50];
                snprintf(str, sizeof(str), "%lld:%lld:%llu:%u:%s",timestamp, timestamp2, (unsigned long long)tag->serial_number, check_num_from_gpio(compartment), locker_name); 

                int msg_id = srv_mqtt_publish("/door/info", str, strlen(str));
                ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d; value: %s", msg_id, str);

                tag = NULL;
                //liga led "liberou pra usar"
                led_cb(SRV_LED_WAITING_ANSWER,0);
                led_cb(SRV_LED_NO_FREE_DOORS,0);
                led_cb(SRV_LED_FREE_TO_USE,1);
                led_cb(SRV_LED_REMOVE_OBJ,0);
                led_cb(SRV_LED_STORE_OBJ,0);

                vTaskDelete(NULL);

            }
            //quando detectar que o ima afastou, começa a contar o tempo

        }
        vTaskDelay(pdMS_TO_TICKS(100));
        
        //quando detectar que o ima aproximou, para de contar o tempo e trava
            //envia via mqtt

    }
    vTaskDelete(NULL);
}

static void split_data(const char* data, int* num, char* locker_name, char *tag_rfid) 
{
    // Use a cópia da string para preservá-la, pois strtok modifica a string original.
    char data_copy[50];
    strncpy(data_copy, data, sizeof(data_copy));

    // Use strtok para dividir a string
    char* token = strtok(data_copy, ":");
    
    if (token != NULL) 
    {
        // Primeiro valor: num (uint8_t)
        *num = (int)atoi(token);
        
        // Segundo valor: locker_name (string)
        token = strtok(NULL, ":");
        if (token != NULL) 
        {
            strcpy(locker_name, token);
            
            // Terceiro valor: tag_rfid (string)
            token = strtok(NULL, ":");
            if (token != NULL) 
            {
                strcpy(tag_rfid, token);
            }
            token = strtok(NULL, ":");
            if (token != NULL) 
            {
                char led[6];
                strcpy(led, token);
                // ESP_LOGI(TAG, "LED: %d - %d", (int)strcmp(led, "store"), (int)strcmp(led, "remove"));

                if(strcmp(led, "store")>=0) 
                {
                    ESP_LOGI(TAG, "STORE");
                    led_cb(SRV_LED_STORE_OBJ, 1);
                }
                else if(strcmp(led, "remove")>=0) 
                {
                    ESP_LOGI(TAG, "REMOVE");
    
                    led_cb(SRV_LED_REMOVE_OBJ, 1);
                }
            }
        }
    }
}