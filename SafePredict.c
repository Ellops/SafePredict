#include "main_config.h"

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "pico/binary_info.h"

#include "math.h"

#include "ssd1306.h"
#include "distance_sr04.h"
#include "dht.h"
#include "rgb_digital.h"
#include "mpu6050.h"
#include "buzzer.h"

#include "http_request.h"
#include "wifi_manager.h"

ssd1306_t oled; 

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

const dht_model_t DHT_MODEL = DHT11;
const uint DHT_DATA_PIN = 17;

dht_t dht;

#if defined(SAFEPREDICT_ONLY_BITDOG_MODE)
    void update_dht() {
        float humidity_increment = ((rand() % 21) - 10) / 10.0;  // Random increment between -1.0 and 1.0
        float temperature_increment = ((rand() % 21) - 10) / 10.0; // Random increment between -1.0 and 1.0
    
        
        float humidity = smoothed_humidity + humidity_increment;
        float temperature = smoothed_temperature + temperature_increment;
    
        if (humidity < 0) humidity = 0;
        if (humidity > 100) humidity = 100;       

        dht_result_t result = DHT_RESULT_OK
        if (result == DHT_RESULT_OK) {
            temperature_buffer[temperature_index_window] = temperature_c;
            temperature_index_window = (temperature_index_window + 1) % DHT_WINDOW_SIZE;
        
            float sum = 0;
            for (int i = 0; i < DHT_WINDOW_SIZE; i++) {
                sum += temperature_buffer[i];
            }
            smoothed_temperature = sum / DHT_WINDOW_SIZE;

            humidity_buffer[humidity_index_window] = humidity;
            humidity_index_window = (humidity_index_window + 1) % DHT_WINDOW_SIZE;
        
            sum = 0;
            for (int i = 0; i < DHT_WINDOW_SIZE; i++) {
                sum += humidity_buffer[i];
            }
            smoothed_humidity = sum / DHT_WINDOW_SIZE;
        }
    }

    void update_current(){
        float current_increment = ((rand() % 21) - 10) / 10.0;
        uint16_t current = smoothed_current + current_increment;
        
        current_buffer[current_index_window] = current;
        current_index_window = (current_index_window + 1) % CURRENT_WINDOW_SIZE;

        float sum = 0;
        for (int i = 0; i < CURRENT_WINDOW_SIZE; i++) {
            sum += current_buffer[i];
        }
        smoothed_current = sum / CURRENT_WINDOW_SIZE;

    }

#else
    void update_dht() {
        dht_start_measurement(&dht);
        
        float humidity;
        float temperature_c;
        dht_result_t result = dht_finish_measurement_blocking(&dht, &humidity, &temperature_c);
        if (result == DHT_RESULT_OK) {
            temperature_buffer[temperature_index_window] = temperature_c;
            temperature_index_window = (temperature_index_window + 1) % DHT_WINDOW_SIZE;
        
            float sum = 0;
            for (int i = 0; i < DHT_WINDOW_SIZE; i++) {
                sum += temperature_buffer[i];
            }
            smoothed_temperature = sum / DHT_WINDOW_SIZE;

            humidity_buffer[humidity_index_window] = humidity;
            humidity_index_window = (humidity_index_window + 1) % DHT_WINDOW_SIZE;
        
            sum = 0;
            for (int i = 0; i < DHT_WINDOW_SIZE; i++) {
                sum += humidity_buffer[i];
            }
            smoothed_humidity = sum / DHT_WINDOW_SIZE;
            
        } else if (result == DHT_RESULT_TIMEOUT) {
            puts("DHT sensor not responding. Please check your wiring.");
        } else {
            assert(result == DHT_RESULT_BAD_CHECKSUM);
            puts("Bad checksum");
        }
    }

    void update_current(){
        adc_select_input(ADC_CHANNEL_2);
        sleep_us(2);
        uint16_t current = adc_read();
        
        current_buffer[current_index_window] = current;
        current_index_window = (current_index_window + 1) % CURRENT_WINDOW_SIZE;

        float sum = 0;
        for (int i = 0; i < CURRENT_WINDOW_SIZE; i++) {
            sum += current_buffer[i];
        }
        smoothed_current = sum / CURRENT_WINDOW_SIZE;

    }
#endif

int main(void){
    stdio_init_all();

    printf("Starting All\n");
    init_oled();
    init_buzzer();
    init_RGB();
    init_sr04();
    init_mpu6050();
    dht_init(&dht, DHT_MODEL, pio0, DHT_DATA_PIN, true /* pull_up */);
    printf("All started\n");
    
    adc_init();
    adc_gpio_init(CURRENT_PIN);

    // wifi_start(WIFI_SSID,WIFI_PASSWORD);

    // play_tone(BUZZER_PIN,400,800);
    
    // thing_send(2,"22");

    // change_color(U32_YELLOW);
    // sleep_ms(45000);
    // thing_send(1,"13");

    
    // sleep_ms(500);
    // change_color(U32_RED);
    // draw_phrase("Finalizado");

    mpu6050_reset();

    while (1) {

        update_vibration();
        printf("Vibration:%f\n",smoothed_vibration);
        update_dht();
        printf("Temperature:%f,Humidity:%f\n",smoothed_temperature,smoothed_humidity);
        update_current();
        printf("Current:%f\n",smoothed_current);

        if(alarm_distance()){
            change_color(U32_RED);
        }
        else{
            change_color(U32_GREEN);
        }

        sleep_ms(500);
    }

    // wifi_end();
    return 0;
}