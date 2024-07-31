#include "esp_log.h"
#include "drv_nvs.h"
#include "drv_webserver.h"
#include <ctype.h>

#define MIN(a,b) (((a)<(b))?(a):(b))  // Definição da macro MIN

static const char *TAG = "drv_webserver";

static esp_err_t config_post_handler(httpd_req_t *req);
static esp_err_t config_get_handler(httpd_req_t *req);
static void url_decode(char *dst, const char *src) ;

static httpd_handle_t server = NULL;

httpd_uri_t config_get_uri = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = config_get_handler,
    .user_ctx = NULL
};

httpd_uri_t config_post_uri = {
    .uri = "/config",
    .method = HTTP_POST,
    .handler = config_post_handler,
    .user_ctx = NULL
};

httpd_handle_t drv_webserver_start(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_register_uri_handler(server, &config_get_uri);
        httpd_register_uri_handler(server, &config_post_uri);
    }
    return server;
}

// Function to stop the web server
void drv_webserver_stop(httpd_handle_t server)
{
    if (server)
    {
        httpd_stop(server);
    }
}

static esp_err_t config_post_handler(httpd_req_t *req)
{
    char buf[512];
    int ret=0, remaining = req->content_len;
     ESP_LOGE(TAG, "content_len: %u", req->content_len);
    while (remaining > 0)
    {
        if ((ret = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)))) <= 0)
        {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT)
            {
                continue;
            }
            return ESP_FAIL;
        }
        remaining -= ret;
    }

    buf[ret] = 0;

    char ssid[32] = {0};
    char ssid_decode[32] = {0};
    char password[64] = {0};
    char password_decode[64] = {0};
    char locker_name[20] = {0};
    char locker_name_decode[20] = {0};
    char num_compartments_str[4] = {0};
    uint8_t num_compartments = 0;

    httpd_query_key_value(buf, "ssid", ssid, sizeof(ssid));
    httpd_query_key_value(buf, "password", password, sizeof(password));
    httpd_query_key_value(buf, "locker_name", locker_name, sizeof(locker_name));
    httpd_query_key_value(buf, "num_compartments", num_compartments_str, sizeof(num_compartments_str));
    ESP_LOGI(TAG, "num compart str: %s", num_compartments_str);

    url_decode(ssid_decode, ssid);
    url_decode(password_decode, password);
    url_decode(locker_name_decode, locker_name);
    num_compartments = (uint8_t)atoi(num_compartments_str);

    drv_nvs_set_str("storage", WIFI_SSID_KEY, ssid_decode);
    drv_nvs_set_str("storage", WIFI_PASS_KEY, password_decode);
    if(strlen(locker_name)>0) drv_nvs_set_str("storage", LOCKER_NAME_KEY, locker_name_decode);
    if(num_compartments > 0) drv_nvs_set_u8("storage", LOCKER_NUM_KEY, num_compartments);

    httpd_resp_send(req, "Configuration Saved", HTTPD_RESP_USE_STRLEN);
    esp_restart();
    return ESP_OK;
}

static esp_err_t config_get_handler(httpd_req_t *req)
{
    char ssid[32] = "";
    char password[64] = "";
    char locker_name[20] = "";
    uint8_t num_compartments = 0;
    size_t ssid_len = sizeof(ssid);
    size_t password_len = sizeof(password);
    size_t locker_name_len = sizeof(locker_name);
    
    drv_nvs_get_str("storage", WIFI_SSID_KEY, ssid, &ssid_len);
    drv_nvs_get_str("storage", WIFI_PASS_KEY, password, &password_len);
    drv_nvs_get_str("storage", LOCKER_NAME_KEY, locker_name, &locker_name_len);
    drv_nvs_get_u8("storage", LOCKER_NUM_KEY, &num_compartments);


    char resp[1024];
    int offset = snprintf(resp, sizeof(resp),
                        "<!DOCTYPE html><html><body>"
                        "<form action=\"/config\" method=\"post\">"
                        "SSID: <input type=\"text\" name=\"ssid\" value=\"%s\"><br>"
                        "Password: <input type=\"text\" name=\"password\" value=\"%s\"><br>"
                        "<input type=\"submit\" value=\"Save\">",
                        ssid, password);

    if (strlen(locker_name) <= 0) {
        ESP_LOGI(TAG, "Locker name: %s", locker_name);
        offset += snprintf(resp + offset, sizeof(resp) - offset,
                        "Locker Name: <input type=\"text\" name=\"locker_name\" value=\"%s\"><br>",
                        locker_name);
    }

    if (num_compartments <= 0) {
        ESP_LOGI(TAG, "Num compartments: %u", num_compartments);
        offset += snprintf(resp + offset, sizeof(resp) - offset,
                        "Número de Compartimentos: <input type=\"number\" name=\"num_compartments\" value=\"%d\"><br>",
                        num_compartments);
    }
    if ((num_compartments <= 0)||(strlen(locker_name) <= 0)) {
    snprintf(resp + offset, sizeof(resp) - offset,
         "<button type=\"submit\">Enviar</button>"
         "</form></body></html>");
    }
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static void url_decode(char *dst, const char *src) 
{
    char a, b;
    while (*src) 
    {
        if ((*src == '%') && ((a = src[1]) && (b = src[2])) && (isxdigit(a) && isxdigit(b))) 
        {
            if (a >= 'a') a -= 'a'-'A';
            if (a >= 'A') a -= ('A' - 10);
            else a -= '0';
            if (b >= 'a') b -= 'a'-'A';
            if (b >= 'A') b -= ('A' - 10);
            else b -= '0';
            *dst++ = 16*a+b;
            src+=3;
        } else if (*src == '+') 
        {
            *dst++ = ' ';
            src++;
        } else 
        {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
}