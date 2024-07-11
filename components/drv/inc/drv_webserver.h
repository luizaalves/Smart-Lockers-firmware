#include "esp_http_server.h"

httpd_handle_t drv_webserver_start(void);

void drv_webserver_stop(httpd_handle_t server);