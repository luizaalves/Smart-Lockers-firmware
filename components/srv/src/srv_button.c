#include "srv_button.h"
#include "srv_mqtt.h"
#include "srv_wifi.h"
#include "drv_webserver.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "time.h"

#define DEBOUNCE_DELAY_MS 10 

uint8_t first_intr = 2;
unsigned long timestamp_ultimo_acionamento = 0;
QueueHandle_t button_queue= NULL;
static int flag = 0;
static const char *TAG = "srv_button";
TaskHandle_t button_handle=NULL;

static void IRAM_ATTR button_isr_handler(void* arg);
static void button_task(void *arg);
static void config_wifi_ap(void);
static void handler_on_ap_got_ip(void *arg, const char* event_base, int32_t event_id, void *event_data);
static bool sensors_detect(void);

void srv_button_init(void)
{
    drv_gpio_set_direction(APP_BUTTON_GPIO, GPIO_MODE_INPUT);
    drv_gpio_set_intr_type(APP_BUTTON_GPIO, GPIO_INTR_ANYEDGE);
    drv_gpio_isr_handler_add(APP_BUTTON_GPIO, button_isr_handler);
    gpio_install_isr_service(0);
    if(button_queue == NULL) button_queue = xQueueCreate(10, sizeof(int));
    if(button_queue == NULL)
    {
        ESP_LOGE(TAG, "Failed to create queue");
        return;
    }

}

static void IRAM_ATTR button_isr_handler(void* arg) 
{
    if(!sensors_detect() && first_intr == 2 && drv_gpio_get_level(APP_BUTTON_GPIO)==0) first_intr = 0;
    else if(!sensors_detect() && first_intr ==0 && drv_gpio_get_level(APP_BUTTON_GPIO))
    {
        unsigned long timestamp = clock();
        if(((timestamp - timestamp_ultimo_acionamento)/CLOCKS_PER_SEC) >= DEBOUNCE_DELAY_MS)
        {
            timestamp_ultimo_acionamento = 0;
            first_intr = 2;
            if(button_handle==NULL) xTaskCreate(button_task, "button_task", 1024*5, NULL, 10, &button_handle);
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            int button_event=1;
            xQueueSendFromISR(button_queue, &button_event, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
        
    }
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
    vTaskDelete(NULL);
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

static bool sensors_detect(void)
{
    return drv_gpio_get_level(APP_SENSOR_1) ||  drv_gpio_get_level(APP_SENSOR_2);
}