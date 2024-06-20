#include "srv_rfid.h"

static rc522_handle_t scanner;

void srv_rfid_start(rfid_config_st rfid_config)
{
    rc522_config_t config = {
        .spi.host = rfid_config.host,
        .spi.miso_gpio = rfid_config.miso,
        .spi.mosi_gpio = rfid_config.mosi,
        .spi.sck_gpio = rfid_config.sck,
        .spi.sda_gpio = rfid_config.sda,
    };

    rc522_create(&config, &scanner);
    rc522_start(scanner);
}

void srv_rfid_set_callback(rfid_event_handler_t handler)
{
    rc522_register_events(scanner, RC522_EVENT_ANY, handler, NULL);
}