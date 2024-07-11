#pragma once

#include "driver/gpio.h" 

typedef struct
{
    uint8_t gpio_num;
    uint8_t gpio_mode;
    uint8_t gpio_intr_type;
    void *handler;
}drv_gpio_config_st;

typedef enum
{
    DRV_GPIO_OK = 0,
    DRV_GPIO_ERR_DIRECTION,
    DRV_GPIO_ERR_INTR_TYPE,
    DRV_GPIO_ERR_ISR_SERVICE,
    DRV_GPIO_ERR_HANDLER,
} drv_gpio_err_et;

drv_gpio_err_et drv_gpio_config(drv_gpio_config_st *config);
