#include "srv_button.h"
#include "srv_mqtt.h"
#include "srv_wifi.h"
#include "drv_webserver.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

QueueHandle_t button_queue= NULL;
static int flag = 0;
static const char *TAG = "srv_button";

static void IRAM_ATTR button_isr_handler(void* arg);
static void button_task(void *arg);
static void config_wifi_ap(void);
static void handler_on_ap_got_ip(void *arg, const char* event_base, int32_t event_id, void *event_data);

void srv_button_init(void)
{
    drv_gpio_config_st config = 
    {
        .gpio_num = APP_BUTTON_GPIO,
        .gpio_mode = GPIO_MODE_INPUT,
        .gpio_intr_type = GPIO_INTR_NEGEDGE,
        .handler = button_isr_handler,
    };

    drv_gpio_config(&config);
    if(button_queue == NULL) button_queue = xQueueCreate(10, sizeof(int));
    if(button_queue == NULL)
    {
        ESP_LOGE(TAG, "Failed to create queue");
        return;
    }
    xTaskCreate(button_task, "button_task", 1024*5, NULL, 10, NULL);

}

static void IRAM_ATTR button_isr_handler(void* arg) 
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    int button_event=1;
    xQueueSendFromISR(button_queue, &button_event, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void button_task(void *arg)
{
    int button_event;
    for(;;)
    {
        // Wait for the event from the ISR
        if ((button_queue!= NULL)&&(xQueueReceive(button_queue, &button_event, portMAX_DELAY) == pdTRUE)&& flag == 0)
        {
            flag = 1;
            // Handle the button press
            srv_mqtt_stop();
            srv_wifi_stop();
            vTaskDelay(pdMS_TO_TICKS(500));
            config_wifi_ap();
            drv_webserver_start();
            // STOP WIFI STA, INIT WIFI AP, INITI WEBSERVER
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void config_wifi_ap(void)
{    
    wifi_config_st config = 
    {
        .ssid = APP_WIFI_AP_SSID,
        .password = APP_WIFI_AP_PASS,
        .mode = DRV_WIFI_MODE_AP,
        .interface = DRV_WIFI_IF_AP
    };
    if(srv_wifi_start(&config) != DRV_WIFI_OK) return;
    srv_wifi_set_callback(handler_on_ap_got_ip, APP_EVENT_IP_AP);
}

static void handler_on_ap_got_ip(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "WIFI_EVENT_AP_STACONNECTED");
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "WIFI_EVENT_AP_STADISCONNECTED");
    }
}
