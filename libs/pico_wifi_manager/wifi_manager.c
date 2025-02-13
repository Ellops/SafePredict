#include "wifi_manager.h"

#define CONNECTION_CHECK_TIME 5000

bool is_connected = false;

static bool wifi_connect(const char* WIFI, const char* PASSWORD){
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI, PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        is_connected = false;
        printf("failed to connect.\n");
        return false;
    }
    is_connected = true;
    printf("Connected.\n");
    return true;
}

bool wifi_start(const char* WIFI, const char* PASSWORD){
    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return false;
    }
    cyw43_arch_enable_sta_mode();
    printf("Connecting to Wi-Fi...\n");
    
    return wifi_connect(WIFI,PASSWORD);
}

bool wifi_end(){
    cyw43_arch_deinit();
    return true;
}


