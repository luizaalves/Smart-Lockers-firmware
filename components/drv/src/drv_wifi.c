#include "drv_wifi.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "esp_log.h"
#include "nvs_flash.h"

static const char* TAG = "DRV_WIFI";

static bool wifi_init_s = false;
static bool wifi_start_s = false;

static void set_default_values(void);
static wifi_err_et set_default_err(esp_err_t err_esp, wifi_err_et error, char *func);

wifi_err_et drv_wifi_init(wifi_config_st *wifi_config_pt)
{
    wifi_config_st wifi_config = {0};
    wifi_config=*wifi_config_pt;
    esp_netif_init();
    esp_event_loop_create_default();
    ///////////////////

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_err_t err = esp_wifi_init(&cfg);
    if(err != ESP_OK) return set_default_err(err, DRV_WIFI_ERR_CONFIG, "esp_wifi_init");

    switch(wifi_config.interface)
    {
    case DRV_WIFI_IF_AP:
        esp_netif_create_default_wifi_ap();
        break;
    case DRV_WIFI_IF_STA:
        esp_netif_inherent_config_t esp_netif_config = ESP_NETIF_INHERENT_DEFAULT_WIFI_STA();

        esp_netif_config.if_desc = wifi_config.interface_description;
        esp_netif_config.route_prio = 128;
        esp_netif_create_wifi(wifi_config.interface, &esp_netif_config);
        esp_wifi_set_default_wifi_sta_handlers();
        break;
    
    default:
        break;
    };

    err = esp_wifi_set_storage(WIFI_STORAGE_RAM);
    if(err != ESP_OK) return set_default_err(err, DRV_WIFI_ERR_STORAGE, "esp_wifi_set_storage");
    err = esp_wifi_set_mode(wifi_config.mode);
    if(err != ESP_OK) return set_default_err(err, DRV_WIFI_ERR_MODE, "esp_wifi_set_mode");

    wifi_config_t wifi_configuration={0};
    switch (wifi_config.interface)
    {
    case DRV_WIFI_IF_STA:
    {
        wifi_configuration.sta.scan_method = WIFI_ALL_CHANNEL_SCAN;
        wifi_configuration.sta.sort_method = WIFI_CONNECT_AP_BY_SIGNAL;
        wifi_configuration.sta.threshold.rssi = -127;
        wifi_configuration.sta.threshold.authmode = WIFI_AUTH_OPEN;

        if (wifi_config.ssid) 
        {
            sprintf(&wifi_configuration.sta.ssid,wifi_config.ssid);
        }

        if (wifi_config.password) 
        {
            sprintf(&wifi_configuration.sta.password,wifi_config.password);
        }

        ESP_LOGI(TAG, "Connecting to %s; %s...", wifi_configuration.sta.ssid, wifi_configuration.sta.password);
        break;
    }
    case DRV_WIFI_IF_AP:
    {
        if (wifi_config.ssid) {
            strncpy((char *)wifi_configuration.ap.ssid, wifi_config.ssid, sizeof(wifi_configuration.ap.ssid) - 1);
            wifi_configuration.ap.ssid[sizeof(wifi_configuration.ap.ssid) - 1] = '\0';
        }

        if (wifi_config.password) {
            strncpy((char *)wifi_configuration.ap.password, wifi_config.password, sizeof(wifi_configuration.ap.password) - 1);
            wifi_configuration.ap.password[sizeof(wifi_configuration.ap.password) - 1] = '\0';
        }
        wifi_configuration.ap.ssid_len = strlen(wifi_config.ssid);
        wifi_configuration.ap.max_connection = 4;
#ifdef CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT
        wifi_configuration.ap.authmode = WIFI_AUTH_WPA3_PSK;
        wifi_configuration.ap.sae_pwe_h2e = WPA3_SAE_PWE_BOTH;
#else /* CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT */
        wifi_configuration.ap.authmode = WIFI_AUTH_WPA2_PSK;
#endif
        wifi_configuration.ap.pmf_cfg.required = true;
        wifi_configuration.ap.channel = 1;

        if (strlen(wifi_config.password) == 0) wifi_configuration.ap.authmode = WIFI_AUTH_OPEN;
        ESP_LOGI(TAG, "Setting up AP with SSID: %s on channel %d...", wifi_configuration.ap.ssid, wifi_configuration.ap.channel);
        wifi_country_t wifi_country = {
            .cc = "BR", // Código do Brasil
            .schan = 1,
            .nchan = 13, // Ajuste o número de canais conforme necessário
            .policy = WIFI_COUNTRY_POLICY_AUTO
        };

        esp_err_t set_country_err = esp_wifi_set_country(&wifi_country);
        if (set_country_err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to set country: %s", esp_err_to_name(set_country_err));
        }
        break;
    }
    default:
        break;
    }
    
    err = esp_wifi_set_config(wifi_config.interface, &wifi_configuration);
    if(err != ESP_OK) return set_default_err( err, DRV_WIFI_ERR_CONFIG, "esp_wifi_set_config");
    err = esp_wifi_start();
    if(err != ESP_OK) return set_default_err( err, DRV_WIFI_ERR_START,"esp_wifi_start");
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

void drv_wifi_stop(void)
{
    esp_wifi_stop();
}

static void set_default_values(void)
{
    wifi_init_s = false;
    wifi_start_s = false;
}

static wifi_err_et set_default_err(esp_err_t err_esp, wifi_err_et error, char *func)
{
    set_default_values();
    ESP_LOGE(TAG, "%s error: %d", func, err_esp);
    return error;
}
