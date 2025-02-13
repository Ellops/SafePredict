#include "pico/stdlib.h"
#include "http_request.h"
#include "wifi_manager.h"
 
#define URL_REQUEST "/update?api_key=&field1="

 int main(void)
 {
    stdio_init_all();
    sleep_ms(5000);

    wifi_start(WIFI_SSID,WIFI_PASSWORD);

    thing_send(2,"35");
    sleep_ms(16000);
    thing_send(1,"25");

    sleep_ms(100);
    wifi_end();
    return 0;
 }
 