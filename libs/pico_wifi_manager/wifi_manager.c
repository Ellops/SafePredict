#include "wifi_manager.h"

bool wifi_start(const char* WIFI, const char* PASSWORD){
    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return false;
    }
    cyw43_arch_enable_sta_mode();
    printf("Connecting to Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI, PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("failed to connect.\n");
        return false;
    }
    printf("Connected.\n");
    return true;
}

bool wifi_end(){
    cyw43_arch_deinit();
    return true;
}


