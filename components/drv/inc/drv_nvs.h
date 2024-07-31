#pragma once
#include <stddef.h>
#include "stdint.h"

#define WIFI_SSID_KEY "wifi_ssid"
#define WIFI_PASS_KEY "wifi_password"
#define LOCKER_NAME_KEY "locker_name"
#define LOCKER_NUM_KEY "locker_num"

typedef enum
{
    DRV_NVS_OK = 0,
    DRV_NVS_ERR_EMPTY,
    DRV_NVS_KEY_NOT_FOUND,
    DRV_NVS_ERR,
    DRV_NVS_ERR_PART_NOT_FOUND
} drv_nvs_err_et;

drv_nvs_err_et drv_nvs_init(void);

drv_nvs_err_et drv_nvs_get_str(const char *namespace_name, const char *key, char *out_value, size_t *length);

drv_nvs_err_et drv_nvs_set_str(const char *namespace_name, const char *key, const char *value);

drv_nvs_err_et drv_nvs_get_u8(const char *namespace_name, const char *key, uint8_t *out_value);

drv_nvs_err_et drv_nvs_set_u8(const char *namespace_name, const char *key, uint8_t value);

drv_nvs_err_et drv_nvs_erase_partition(const char *namespace_name);

drv_nvs_err_et drv_nvs_erase_all(void);
