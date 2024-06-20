#include "drv_wifi.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "esp_log.h"
#include "nvs_flash.h"

static const char* TAG = "DRV_WIFI";

static bool wifi_init_s = false;
static bool wifi_start_s = false;
static esp_netif_t *wifi_netif_s = NULL;
static wifi_config_st wifi_config_s = {0};

static void set_default_values(void);
static wifi_err_et set_default_err(esp_err_t err_esp, wifi_err_et error, char *func);

wifi_err_et drv_wifi_init(wifi_config_st wifi_config)
{
    //////////remover daqui
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ///////////////////

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_err_t err = esp_wifi_init(&cfg);
    if(err != ESP_OK) return set_default_err(err, DRV_WIFI_ERR_CONFIG, "esp_wifi_init");

    esp_netif_inherent_config_t esp_netif_config = ESP_NETIF_INHERENT_DEFAULT_WIFI_STA();

    esp_netif_config.if_desc = wifi_config.interface_description;
    esp_netif_config.route_prio = 128;
    wifi_netif_s = esp_netif_create_wifi(wifi_config.interface, &esp_netif_config);
    switch (wifi_config.interface)
    {
    case DRV_WIFI_IF_AP:
        esp_wifi_set_default_wifi_ap_handlers();
        break;
    case DRV_WIFI_IF_STA:
        esp_wifi_set_default_wifi_sta_handlers();
        break;
    
    default:
        esp_wifi_set_default_wifi_nan_handlers();
        break;
    };

    err = esp_wifi_set_storage(WIFI_STORAGE_RAM);
    if(err != ESP_OK) return set_default_err(err, DRV_WIFI_ERR_STORAGE, "esp_wifi_set_storage");

    err = esp_wifi_set_mode(wifi_config.mode);
    if(err != ESP_OK) return set_default_err(err, DRV_WIFI_ERR_MODE, "esp_wifi_set_mode");

    err = esp_wifi_start();
    if(err != ESP_OK)  return set_default_err(err, DRV_WIFI_ERR_START, "esp_wifi_start");

    wifi_init_s = true;
    wifi_config_s = wifi_config;
    return DRV_WIFI_OK;
}

wifi_err_et drv_wifi_start(void)
{
    if(!wifi_init_s) return set_default_err(DRV_WIFI_ERR_INIT, DRV_WIFI_ERR_INIT, "wifi_init is not set.");

    esp_err_t err = esp_wifi_start();
    if(err != ESP_OK) return set_default_err( err, DRV_WIFI_ERR_START,"esp_wifi_start");
    
    wifi_config_t wifi_config = {
        .sta = {
            .scan_method = WIFI_ALL_CHANNEL_SCAN,
            .sort_method = WIFI_CONNECT_AP_BY_SIGNAL,
            .threshold.rssi = -127,
            .threshold.authmode = WIFI_AUTH_OPEN,
        },
    };

    if (wifi_config_s.ssid) {
        strncpy((char *)wifi_config.sta.ssid, wifi_config_s.ssid, sizeof(wifi_config.sta.ssid) - 1);
        wifi_config.sta.ssid[sizeof(wifi_config.sta.ssid) - 1] = '\0';
    }

    if (wifi_config_s.password) {
        strncpy((char *)wifi_config.sta.password, wifi_config_s.password, sizeof(wifi_config.sta.password) - 1);
        wifi_config.sta.password[sizeof(wifi_config.sta.password) - 1] = '\0';
    }

    ESP_LOGI(TAG, "Connecting to %s...", wifi_config.sta.ssid);
    err = esp_wifi_set_config(wifi_config_s.interface, &wifi_config);
    if(err != ESP_OK) return set_default_err( err, DRV_WIFI_ERR_CONFIG, "esp_wifi_set_config");
    
    wifi_start_s = true;

    return DRV_WIFI_OK;
}

wifi_err_et drv_wifi_connect(void)
{
    if(!wifi_start_s) return set_default_err(DRV_WIFI_ERR_START, DRV_WIFI_ERR_START, "wifi_start is not set.");
    esp_err_t err = esp_wifi_connect();
    if(err != ESP_OK) return set_default_err(err, DRV_WIFI_ERR_CONNECT, "esp_wifi_connect");

    return DRV_WIFI_OK;
}

void drv_wifi_set_callback(wifi_event_et evt_id, wifi_event_handler_t handler) 
{
    esp_event_base_t evt_base = WIFI_EVENT;
    esp_event_handler_register(evt_id == DRV_WIFI_IP_STA ? IP_EVENT: evt_base, (int32_t) evt_id, handler, NULL);
}

static void set_default_values(void)
{
    wifi_init_s = false;
    wifi_start_s = false;
    wifi_netif_s = NULL;
    wifi_config_s.ssid = NULL;
    wifi_config_s.password = NULL;
    wifi_config_s.mode = DRV_WIFI_MODE_NULL;
    wifi_config_s.interface = DRV_WIFI_INTERFACE_NULL;
    wifi_config_s.interface_description = NULL;
}

static wifi_err_et set_default_err(esp_err_t err_esp, wifi_err_et error, char *func)
{
    set_default_values();
    ESP_LOGE(TAG, "%s error: %d", func, err_esp);
    return error;
}
