 #include "pico/cyw43_arch.h"
 #include "pico/stdlib.h"
 
 int main(void)
 {
    stdio_init_all();
    sleep_ms(5000);
    
    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
    }

    cyw43_arch_enable_sta_mode();
    printf("Connecting to Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("failed to connect.\n");
        exit(1);
    } else {
        printf("Connected.\n");
    }

    cyw43_arch_deinit();
    return 0;
 }
 