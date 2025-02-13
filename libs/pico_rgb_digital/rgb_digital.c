#include "mpu6050.h"

#define RED_PIN 13
#define GREEN_PIN 11
#define BLUE_PIN 12

void init_RGB(){
    gpio_init(RED_PIN);
    gpio_set_dir(RED_PIN, GPIO_OUT);
    gpio_init(GREEN_PIN);
    gpio_set_dir(GREEN_PIN, GPIO_OUT);
    gpio_init(BLUE_PIN);
    gpio_set_dir(BLUE_PIN, GPIO_OUT);
}

void change_color(uint32_t color){
    uint8_t red = (color >> 8) & 0xFF;
    uint8_t green = (color >> 16) & 0xFF;
    uint8_t blue = color & 0xFF;

    gpio_put(RED_PIN, red); 
    gpio_put(GREEN_PIN, green);
    gpio_put(BLUE_PIN, blue);
}
