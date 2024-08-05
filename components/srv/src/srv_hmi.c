#include "srv_hmi.h"
#include "stdio.h"
#include "drv_gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// Declaração do semáforo e da task
SemaphoreHandle_t blink_semaphore = NULL;
TaskHandle_t blink_led_task_handle = NULL;

static void blink_led(uint8_t num);
static void stop_blink_led(uint8_t num);
static void blink_led_task(void *pvParameters);

void srv_hmi_init(void)
{
    if(blink_semaphore == NULL) blink_semaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(blink_semaphore);
    blink_led_task_handle = NULL;

    drv_gpio_set_direction(SRV_LED_NO_FREE_DOORS, GPIO_MODE_OUTPUT);
    drv_gpio_set_direction(SRV_LED_WAITING_ANSWER, GPIO_MODE_OUTPUT);
    drv_gpio_set_direction(SRV_LED_FREE_TO_USE, GPIO_MODE_OUTPUT);
    drv_gpio_set_direction(SRV_LED_STORE_OBJ, GPIO_MODE_OUTPUT);
    drv_gpio_set_direction(SRV_LED_TAG_NOT_FOUND_IN_DB, GPIO_MODE_OUTPUT);
    drv_gpio_set_level(SRV_LED_NO_FREE_DOORS, 0);
    drv_gpio_set_level(SRV_LED_WAITING_ANSWER, 0);
    drv_gpio_set_level(SRV_LED_FREE_TO_USE, 0);
    drv_gpio_set_level(SRV_LED_STORE_OBJ, 0);
    drv_gpio_set_level(SRV_LED_TAG_NOT_FOUND_IN_DB, 0);
}
void handler_hmi(uint8_t data, uint8_t status)
{
    switch (data)
    {
    case SRV_LED_NO_FREE_DOORS:
    case SRV_LED_WAITING_ANSWER:
    case SRV_LED_FREE_TO_USE:
    case SRV_LED_TAG_NOT_FOUND_IN_DB:
        drv_gpio_set_level(data, status);
        break;
    case SRV_LED_STORE_OBJ:
        stop_blink_led(data);
        // vTaskDelay(pdMS_TO_TICKS(100));
        drv_gpio_set_level(data, status);
        break;
    case SRV_LED_REMOVE_OBJ:
        if(status) blink_led(SRV_LED_STORE_OBJ);
        else stop_blink_led(SRV_LED_STORE_OBJ);
        break;
    default:
        break;
    }
}

static void blink_led(uint8_t num)
{
    eTaskState state_task = blink_led_task_handle != NULL ? eTaskGetState(blink_led_task_handle): eInvalid;
    if(state_task!=eRunning)  xTaskCreate(blink_led_task, "Blink", 1024*5, (void *)num, 10, &blink_led_task_handle);
    xSemaphoreGive(blink_semaphore);
}

static void stop_blink_led(uint8_t num)
{
    if (blink_led_task_handle != NULL) 
    {
        vTaskDelete(blink_led_task_handle);
        blink_led_task_handle = NULL;
        xSemaphoreGive(blink_semaphore);
    }
}

static void blink_led_task(void *pvParameters) 
{
    uint8_t num = pvParameters;

    for(;;)
    {
        if (xSemaphoreTake(blink_semaphore, portMAX_DELAY) == pdTRUE) 
        {
            gpio_set_level(num, 1);
            vTaskDelay(pdMS_TO_TICKS(500));
            gpio_set_level(num, 0);
            vTaskDelay(pdMS_TO_TICKS(500));
            xSemaphoreGive(blink_semaphore);
        }
    }
}