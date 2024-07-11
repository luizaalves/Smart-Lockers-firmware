#pragma once
#include <stddef.h>

#define WIFI_SSID_KEY "wifi_ssid"
#define WIFI_PASS_KEY "wifi_password"

typedef enum
{
    DRV_NVS_OK = 0,
    DRV_NVS_ERR_EMPTY,
    DRV_NVS_KEY_NOT_FOUND,
    DRV_NVS_ERR,
    DRV_NVS_ERR_PART_NOT_FOUND
} drv_nvs_err_et;

drv_nvs_err_et drv_nvs_init(void);

drv_nvs_err_et drv_nvs_get(const char *namespace_name, const char *key, char *out_value, size_t *length);

drv_nvs_err_et drv_nvs_set(const char *namespace_name, const char *key, const char *value);

drv_nvs_err_et drv_nvs_erase_partition(const char *namespace_name);

drv_nvs_err_et drv_nvs_erase_all(void);
