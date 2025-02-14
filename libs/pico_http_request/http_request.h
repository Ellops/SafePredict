/**
 * Copyright (c) 2024 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

 #ifndef HTTP_REQUEST_H
 #define HTTP_REQUEST_H
 
 #include "lwip/apps/http_client.h"
 #include <string.h>
 
 typedef enum {
    THINGSPEAK_BREAK=1,
    THINGSPEAK_CURRENT,
    THINGSPEAK_VIBRATION,
    THINGSPEAK_TEMPERATURE,
    THINGSPEAK_HUMIDITY
} thingspeak_type_t;

typedef struct {
    thingspeak_type_t type;
    char value[8];
} queue_entry_t;

 int thing_send(queue_entry_t data_send);

 #endif
 