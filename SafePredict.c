#include "main_config.h"

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"


#include "ssd1306.h"
#include "distance_sr04.h"
#include "dht.h"

#include "http_request.h"
#include "wifi_manager.h"

#define BUZZER_PIN 21

#define RED_PIN 13
#define GREEN_PIN 11
#define BLUE_PIN 12

static const dht_model_t DHT_MODEL = DHT11;
static const uint DATA_PIN = 17;

#define URGB_U32(r, g, b) \
    ((uint32_t)((r) << 8) | \
     (uint32_t)((g) << 16) | \
     (uint32_t)(b))

#define U32_RED URGB_U32(0xFF, 0, 0)
#define U32_YELLOW URGB_U32(0xFF, 0xFF, 0)
#define U32_GREEN URGB_U32(0, 0xFF, 0)

ssd1306_t oled; //Estrutura de dados do oled

void init_buzzer(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 4.0f); // Ajusta divisor de clock
    pwm_init(slice_num, &config, true);
    pwm_set_gpio_level(pin, 0); // Desliga o PWM inicialmente
}

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

void play_tone(uint pin, uint frequency, uint duration_ms) {
    uint slice_num = pwm_gpio_to_slice_num(pin);
    uint32_t clock_freq = clock_get_hz(clk_sys);
    uint32_t top = clock_freq / frequency - 1;

    pwm_set_wrap(slice_num, top);
    pwm_set_gpio_level(pin, top / 2); // 50% de duty cycle

    sleep_ms(duration_ms);

    pwm_set_gpio_level(pin, 0); // Desliga o som após a duração
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
    init_buzzer(BUZZER_PIN);
    init_RGB();
    init_sr04();
    init_mpu6050();

    sleep_ms(5000);


    // dht_t dht;
    // dht_init(&dht, DHT_MODEL, pio0, DATA_PIN, true /* pull_up */);

    // change_color(U32_GREEN);
    
    draw_phrase("Iniciando");
    // sleep_ms(5000);
    // change_color(U32_RED);

    // wifi_start(WIFI_SSID,WIFI_PASSWORD);
    draw_phrase("Connectado");
    // play_tone(BUZZER_PIN,400,800);
    
    // thing_send(2,"22");
    draw_phrase("Thing 2 Enviado");
    // change_color(U32_YELLOW);
    // sleep_ms(45000);
    draw_phrase("Thing 1 Enviado");
    // thing_send(1,"13");

    
    // sleep_ms(500);
    // change_color(U32_RED);
    // draw_phrase("Finalizado");

    // while(true){
    //     dht_start_measurement(&dht);
    
    //     float humidity;
    //     float temperature_c;
    //     dht_result_t result = dht_finish_measurement_blocking(&dht, &humidity, &temperature_c);
    //     if (result == DHT_RESULT_OK) {
    //         printf("%.1f C , %.1f%% humidity\n", temperature_c, humidity);
    //     } else if (result == DHT_RESULT_TIMEOUT) {
    //         puts("DHT sensor not responding. Please check your wiring.");
    //     } else {
    //         assert(result == DHT_RESULT_BAD_CHECKSUM);
    //         puts("Bad checksum");
    //     }
    //     sleep_ms(2000);
    // }


    mpu6050_reset();

    int16_t acceleration[3], gyro[3], temp;

    while (1) {
        draw_phrase("1");
        sleep_ms(1000);
        mpu6050_read_raw(acceleration, gyro, &temp);
        draw_phrase("2");
        // These are the raw numbers from the chip, so will need tweaking to be really useful.
        // See the datasheet for more information
        printf("Acc. X = %d, Y = %d, Z = %d\n", acceleration[0], acceleration[1], acceleration[2]);
        printf("Gyro. X = %d, Y = %d, Z = %d\n", gyro[0], gyro[1], gyro[2]);
        // Temperature is simple so use the datasheet calculation to get deg C.
        // Note this is chip temperature.
        printf("Temp. = %f\n", (temp / 340.0) + 36.53);
        
        if(alarm_distance()){
            printf("Alarme\n");
        }

        sleep_ms(1000);
    }

    // wifi_end();
    return 0;
}