#include <stdio.h>
#include "esp_log.h"
#include "srv_communication.h"

static const char TAG[] = "main";

void app_main(void)
{
    ESP_LOGI(TAG, "HELLO WORLD");
    srv_comm_init();
    // srv_comm_config_wifi();
}
