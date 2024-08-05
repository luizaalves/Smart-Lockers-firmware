#pragma once

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*hmi_cb)(uint8_t data, uint8_t status);

#define SRV_LED_NO_FREE_DOORS 16
#define SRV_LED_WAITING_ANSWER 5
#define SRV_LED_FREE_TO_USE 17
#define SRV_LED_STORE_OBJ 4
#define SRV_LED_REMOVE_OBJ 2
#define SRV_LED_TAG_NOT_FOUND_IN_DB 22

void srv_hmi_init(void);

void handler_hmi(uint8_t data, uint8_t status);

#ifdef __cplusplus
}
#endif
