#include "drv_gpio.h"

drv_gpio_err_et drv_gpio_set_direction(uint8_t gpio_num, uint8_t gpio_mode)
{
    esp_err_t err = gpio_set_direction(gpio_num, gpio_mode);
    if(err != ESP_OK) return DRV_GPIO_ERR_DIRECTION;
    if(gpio_mode == GPIO_MODE_INPUT) gpio_set_pull_mode(gpio_num, GPIO_PULLUP_ONLY);
    return DRV_GPIO_OK;
}

drv_gpio_err_et drv_gpio_set_intr_type(uint8_t gpio_num, uint8_t gpio_intr_type)
{
    esp_err_t err = gpio_set_intr_type(gpio_num, gpio_intr_type);
    if(err != ESP_OK) return DRV_GPIO_ERR_INTR_TYPE;
    err = gpio_install_isr_service(0);
    if(err != ESP_OK) return DRV_GPIO_ERR_ISR_SERVICE;
    return DRV_GPIO_OK;
}

drv_gpio_err_et drv_gpio_isr_handler_add(uint8_t gpio_num, void * handler)
{
    esp_err_t err = gpio_isr_handler_add(gpio_num, handler, (void * ) gpio_num);
    if(err != ESP_OK) return DRV_GPIO_ERR_HANDLER;
    return DRV_GPIO_OK;
}

drv_gpio_err_et drv_gpio_set_level(uint8_t num, uint32_t on_off)
{
    drv_gpio_err_et err = gpio_set_level(num, on_off) == ESP_OK ? DRV_GPIO_OK: DRV_GPIO_ERR_INVALID_NUM;
    return err;
}

uint8_t drv_gpio_get_level(uint8_t num)
{
    return gpio_get_level(num);
}

drv_gpio_err_et drv_gpio_set_pull(uint8_t num)
{
    gpio_set_pull_mode(num, GPIO_PULLDOWN_ENABLE);
    gpio_set_pull_mode(num, GPIO_PULLUP_DISABLE);
    return DRV_GPIO_OK;
}