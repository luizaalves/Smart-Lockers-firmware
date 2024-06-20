#include <stdio.h>
#include "srv_wifi.h"
#include "esp_log.h"

static const char* TAG = "SRV_WIFI";

static bool wifi_start_s = false;

wifi_err_et srv_wifi_start(wifi_config_st config)
{
    wifi_err_et err = drv_wifi_init(config);
    if(err != DRV_WIFI_OK) return err;
    err = drv_wifi_start();
    if(err != DRV_WIFI_OK) return err;

    wifi_start_s = true;
    
    return DRV_WIFI_OK;
}

wifi_err_et srv_wifi_connect(void)
{
    if(!wifi_start_s) 
    {
        ESP_LOGE(TAG, "Wi-fi not started error.");
        wifi_start_s = false;
        return DRV_WIFI_ERR_CONNECT;
    }

    drv_wifi_connect();
    //drv_wifi_register_event_handler(DRV_WIFI_IP_STA, handler_on_sta_got_ip);
    return DRV_WIFI_OK;
}

void srv_wifi_set_callback(wifi_event_handler_t handler, wifi_event_et event_id)
{
    drv_wifi_set_callback(event_id, (void*)handler);
}