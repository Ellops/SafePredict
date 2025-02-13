#include "oledconfig.h"

#include "pico/stdlib.h"

#include "ssd1306.h"

#include "http_request.h"
#include "wifi_manager.h"

ssd1306_t oled; //Estrutura de dados do oled

void init_oled() {
    //Inicialização do I2c
    i2c_init(i2c1, 100 * 1000); 
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C); 
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C); 
    gpio_pull_up(I2C_SDA_PIN); 
    gpio_pull_up(I2C_SCL_PIN);

    //Inicialização do OLED 
    ssd1306_init(&oled, OLED_WIDTH, OLED_HEIGHT, 0x3C, i2c1); 
    ssd1306_clear(&oled); 
    ssd1306_show(&oled); 
} 

void draw_phrase(const char* text) {

    ssd1306_clear(&oled);

    int text_length = strlen(text);
    int x = GET_CENTER_LINE(text_length);
    int y = (OLED_HEIGHT - FONT_HEIGHT) / 2;
    ssd1306_draw_string(&oled, x, y, 1, text);

   
    ssd1306_show(&oled); 
}

int main(void){
    stdio_init_all();
    init_oled();

    draw_phrase("Iniciando");
    sleep_ms(5000);

    wifi_start(WIFI_SSID,WIFI_PASSWORD);
    draw_phrase("Connectado");

    thing_send(2,"22");
    draw_phrase("Thing 2 Enviado");
    sleep_ms(45000);
    draw_phrase("Thing 1 Enviado");
    thing_send(1,"13");

    sleep_ms(100);
    wifi_end();
    return 0;
}
 