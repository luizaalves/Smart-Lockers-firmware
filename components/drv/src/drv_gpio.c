#include "drv_gpio.h"

drv_gpio_err_et drv_gpio_config(drv_gpio_config_st *config)
{
    esp_err_t err = gpio_set_direction(config->gpio_num, config->gpio_mode);
    if(err != ESP_OK) return DRV_GPIO_ERR_DIRECTION;
    err = gpio_set_intr_type(config->gpio_num, config->gpio_intr_type);
    if(err != ESP_OK) return DRV_GPIO_ERR_INTR_TYPE;
    err = gpio_install_isr_service(0);
    if(err != ESP_OK) return DRV_GPIO_ERR_ISR_SERVICE;
    err = gpio_isr_handler_add(config->gpio_num, config->handler, NULL);
    if(err != ESP_OK) return DRV_GPIO_ERR_HANDLER;
    return DRV_GPIO_OK;
}