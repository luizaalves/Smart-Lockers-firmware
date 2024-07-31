#pragma once

#include "driver/gpio.h" 

typedef struct
{
    uint8_t gpio_num;
    uint8_t gpio_mode;
    uint8_t gpio_intr_type;
    void *handler;
} drv_gpio_config_st;

typedef enum
{
    DRV_GPIO_OK = 0,
    DRV_GPIO_ERR_DIRECTION,
    DRV_GPIO_ERR_INTR_TYPE,
    DRV_GPIO_ERR_ISR_SERVICE,
    DRV_GPIO_ERR_HANDLER,
    DRV_GPIO_ERR_INVALID_NUM,
} drv_gpio_err_et;

drv_gpio_err_et drv_gpio_set_direction(uint8_t gpio_num, uint8_t gpio_mode);

drv_gpio_err_et drv_gpio_set_intr_type(uint8_t gpio_num, uint8_t gpio_intr_type);

drv_gpio_err_et drv_gpio_isr_handler_add(uint8_t gpio_num, void * handler);

drv_gpio_err_et drv_gpio_set_level(uint8_t num, uint32_t on_off);

uint8_t drv_gpio_get_level(uint8_t num);

drv_gpio_err_et drv_gpio_set_pull(uint8_t num);