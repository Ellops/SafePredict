
#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

extern bool is_connected; //Tentativa de ter uma flag de status para o wifi
/*
    Utilizando a arquitetura cyw43_arch com simplificações
*/
bool wifi_start(const char* WIFI, const char* PASSWORD);//Inicia o WIFI
bool wifi_end();//Finaliza WIFI

#endif