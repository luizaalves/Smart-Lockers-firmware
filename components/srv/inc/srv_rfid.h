#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "rc522.h"

typedef struct 
{
    uint8_t host;
    uint8_t miso;
    uint8_t mosi;
    uint8_t sck;
    uint8_t sda;
} rfid_config_st;

typedef void (*rfid_event_handler_t)(void* event_handler_arg,
                                        const char* event_base,
                                        int32_t event_id,
                                        void* event_data); 

void srv_rfid_start(rfid_config_st rfid_config);

void srv_rfid_set_callback(rfid_event_handler_t handler);

#ifdef __cplusplus
}
#endif
