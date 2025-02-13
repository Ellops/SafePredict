
#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "pico/cyw43_arch.h"

bool wifi_start(const char* WIFI, const char* PASSWORD);
bool wifi_end();

#endif