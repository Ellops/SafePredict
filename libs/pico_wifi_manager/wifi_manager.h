
#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

extern bool is_connected;

bool wifi_start(const char* WIFI, const char* PASSWORD);
void wifi_monitor(const char* WIFI, const char* PASSWORD);
bool wifi_end();

#endif