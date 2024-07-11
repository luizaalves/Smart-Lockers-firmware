#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_wifi.h"

typedef enum
{
    DRV_WIFI_OK = 0,
    DRV_WIFI_ERR_CONFIG,
    DRV_WIFI_ERR_STORAGE,
    DRV_WIFI_ERR_MODE,
    DRV_WIFI_ERR_INIT,
    DRV_WIFI_ERR_START,
    DRV_WIFI_ERR_CONNECT

} wifi_err_et;

typedef enum
{
    DRV_WIFI_MODE_STA = WIFI_MODE_STA,
    DRV_WIFI_MODE_AP = WIFI_MODE_AP,
    DRV_WIFI_MODE_APSTA = WIFI_MODE_APSTA,
    DRV_WIFI_MODE_NULL
} wifi_mode_et;

typedef enum
{
    DRV_WIFI_IF_STA = WIFI_IF_STA,
    DRV_WIFI_IF_AP = WIFI_IF_AP,
    DRV_WIFI_INTERFACE_NULL
} wifi_interface_et;

typedef enum
{
    DRV_WIFI_IP_STA = IP_EVENT_STA_GOT_IP,
    DRV_WIFI_IP_AP = ESP_EVENT_ANY_ID
} wifi_event_et;

typedef struct 
{
    char *ssid;
    char *password;
    wifi_mode_et mode;
    wifi_interface_et interface;
    char * interface_description;
} wifi_config_st;

typedef void (*wifi_event_handler_t)(void* event_handler_arg,
                                        const char* event_base,
                                        int32_t event_id,
                                        void* event_data); 

/**
 * @brief Wi-fi initialization
 * 
 * @param wifi_config – Network's information [Struct: SSID, PASSWORD, etc.]
 * @return wifi_err_et Return DRV_WIFI_ERR_CONFIG   if wi-fi init fails,
 *                            DRV_WIFI_ERR_STORAGE  if wi-fi set storage fails,
 *                            DRV_WIFI_ERR_MODE     if wi-fi set mode fails,
 *                            DRV_WIFI_ERR_START    if wi-fi start fails or
 *                            DRV_WIFI_OK           if succeed
 */
wifi_err_et drv_wifi_init(wifi_config_st *wifi_config_pt);
wifi_err_et drv_wifi_open_ap(wifi_config_st config);


/**
 * @brief Connect wi-fi
 * 
 * @return wifi_err_et return DRV_WIFI_ERR_INIT     if wi-fi to started,
 *                            DRV_WIFI_ERR_CONNECT  if wi-fi connect fails or
 *                            DRV_WIFI_OK           if succeed
 */
wifi_err_et drv_wifi_connect(void);

/**
 * @brief Register callback
 * 
 * @param evt_id – the ID of the event to register the handler for
 * @param handler – the handler function which gets called when the event is dispatched
 */
void drv_wifi_set_callback(wifi_event_et evt_id, wifi_event_handler_t handler);

void drv_wifi_stop(void);

#ifdef __cplusplus
}
#endif
