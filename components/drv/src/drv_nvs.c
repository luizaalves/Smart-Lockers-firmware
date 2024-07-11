#include "drv_nvs.h"
#include "nvs_flash.h"
#include "esp_log.h"

static const char *TAG = "drv_nvs";

drv_nvs_err_et drv_nvs_init(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        nvs_flash_erase();
        err = nvs_flash_init();
        if(err != ESP_OK) return DRV_NVS_ERR;
    }
    return DRV_NVS_OK;
}

drv_nvs_err_et drv_nvs_get(const char *namespace_name, const char *key, char *out_value, size_t *length)
{
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open(namespace_name, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) 
    {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return DRV_NVS_ERR_EMPTY;
    }  
    err = nvs_get_str(my_handle, key, out_value, length);
    if (err == ESP_ERR_NVS_NOT_FOUND) 
    {
        ESP_LOGW(TAG, "key not found in NVS");
        length = 0;
        return DRV_NVS_KEY_NOT_FOUND;
    } 
    else if (err != ESP_OK) 
    {
        ESP_LOGE(TAG, "Error (%s) reading key from NVS!", esp_err_to_name(err));
        return DRV_NVS_ERR;
    }
    nvs_close(my_handle);
    return DRV_NVS_OK;
}

drv_nvs_err_et drv_nvs_set(const char *namespace_name, const char *key, const char *value)
{
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open(namespace_name, NVS_READWRITE, &my_handle);
    err = nvs_set_str(my_handle, key, value);
    if(err != ESP_OK) return DRV_NVS_ERR;
    printf("Committing updates in NVS ... ");
    err = nvs_commit(my_handle);
    if(err != ESP_OK) return DRV_NVS_ERR;
    printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
    nvs_close(my_handle);
    return DRV_NVS_OK;
}

drv_nvs_err_et drv_nvs_erase_partition(const char *namespace_name)
{
    esp_err_t err = nvs_flash_erase_partition(namespace_name);
    if(err != ESP_OK) return DRV_NVS_ERR_PART_NOT_FOUND;
    return DRV_NVS_OK;
}

drv_nvs_err_et drv_nvs_erase_all(void)
{
    esp_err_t err = nvs_flash_erase();
    if(err != ESP_OK) return DRV_NVS_ERR;
    return DRV_NVS_OK;
}