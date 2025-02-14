#include "main_config.h"

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "pico/binary_info.h"
#include "pico/util/queue.h"

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

void draw_menu(float temperature, float humidity, float current, float vibration) {
    ssd1306_clear(&oled);
    // Maximum number of characters per line
    int x = 0; // Starting at the beginning of each line
    int y = 0; // Initial Y position (top of the screen)

    // Line 1: Temperature
    char temp_str[20];
    snprintf(temp_str, sizeof(temp_str), "T: %.2fC, H: %.2f", temperature,humidity);
    ssd1306_draw_string(&oled, x, y, 1, temp_str);
    y += FONT_HEIGHT + SPACING;  // Move to the next line


    // Line 3: Current
    char current_str[20];
    snprintf(current_str, sizeof(current_str), "Cur: %.2f A", current);
    ssd1306_draw_string(&oled, x, y, 1, current_str);
    y += FONT_HEIGHT + SPACING;

    // Line 4: Vibration
    char vibration_str[20];
    snprintf(vibration_str, sizeof(vibration_str), "Vib: %.2f Mod", vibration);
    ssd1306_draw_string(&oled, x, y, 1, vibration_str);
    ssd1306_show(&oled);
}

const dht_model_t DHT_MODEL = DHT11;
const uint DHT_DATA_PIN = 17;

dht_t dht;

#if SAFEPREDICT_ONLY_BITDOG_MODE
    #include <time.h>
    void update_dht() {
        float humidity_increment = ((rand() % 21) - 10) / 10.0;  // Random increment between -1.0 and 1.0
        float temperature_increment = ((rand() % 21) - 10) / 10.0; // Random increment between -1.0 and 1.0
    
        
        float humidity = smoothed_humidity + humidity_increment;
        float temperature = smoothed_temperature + temperature_increment;
    
        if (humidity < 0) humidity = 0;
        if (humidity > 100) humidity = 100;       

        dht_result_t result = DHT_RESULT_OK;
        if (result == DHT_RESULT_OK) {
            temperature_buffer[temperature_index_window] = temperature;
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
        float current = ((adc_read() - 2048) / 2048.0) * 20.0;
        
        current_buffer[current_index_window] = current;
        current_index_window = (current_index_window + 1) % CURRENT_WINDOW_SIZE;

        float sum = 0;
        for (int i = 0; i < CURRENT_WINDOW_SIZE; i++) {
            sum += current_buffer[i];
        }
        smoothed_current = sum / CURRENT_WINDOW_SIZE;

    }
#endif

bool motor_status = true;
uint deployed_breaks = 0;

int init_motor(){
    gpio_init(MOTOR_PIN);
    gpio_set_dir(MOTOR_PIN, GPIO_OUT);    
}

void deploy_break(){
    if(motor_status){
        gpio_put(MOTOR_PIN,1);
    }
    else{
        gpio_put(MOTOR_PIN,0);
    }
}

queue_t thingspeak_queue;
queue_entry_t data_to_send;
volatile bool send_data = false;
volatile uint32_t last_data_collection = 0;

void enqueue_data(thingspeak_type_t type, float value) {
    queue_entry_t entry;
    entry.type = type;
    snprintf(entry.value, sizeof(entry.value), "%.3f", value);
    if (!queue_try_add(&thingspeak_queue, &entry)) {
        printf("Queue is full!\n");
    }
}

bool enqueue_timer_callback(struct repeating_timer *t) {
    static int counter = 0;
    thingspeak_type_t type;
    float value = 0.0f;
    switch (counter % 5) {
        case 0: 
            type = THINGSPEAK_BREAK; 
            value = deployed_breaks;
            if(value){
                deployed_breaks = 0;
            }
            break;
        case 1:
            type = THINGSPEAK_CURRENT;
            value = smoothed_current;
            break;
        case 2: 
            type = THINGSPEAK_VIBRATION; 
            value = smoothed_vibration;
            break;
        case 3: 
            type = THINGSPEAK_TEMPERATURE;
            value = smoothed_temperature;
            break;
        case 4: 
            type = THINGSPEAK_HUMIDITY;
            value = smoothed_humidity;
            break;
    }
    enqueue_data(type, value);
    counter++;
    return true;
}

bool dequeue_timer_callback(struct repeating_timer *t) {
    queue_try_remove(&thingspeak_queue, &data_to_send);
    send_data = true;
    return true;
}

int main(void){
    #if defined(SAFEPREDICT_ONLY_BITDOG_MODE)
    srand(time(NULL));
    #endif
    stdio_init_all();

    init_oled();
    init_buzzer();
    init_RGB();
    init_sr04();
    init_mpu6050();
    dht_init(&dht, DHT_MODEL, pio0, DHT_DATA_PIN, true);
    init_motor();
    adc_init();
    adc_gpio_init(CURRENT_PIN);
    queue_init(&thingspeak_queue, sizeof(queue_entry_t), 10);

    mpu6050_reset();

    struct repeating_timer enqueue_timer;
    struct repeating_timer dequeue_timer;

    add_repeating_timer_ms(18000, enqueue_timer_callback, NULL, &enqueue_timer);
    add_repeating_timer_ms(20000, dequeue_timer_callback, NULL, &dequeue_timer);

    wifi_start(WIFI_SSID,WIFI_PASSWORD);
    
    while (1) {
        uint32_t current_time = to_ms_since_boot(get_absolute_time());
        
        if (current_time - last_data_collection > 1000){
            last_data_collection = current_time;
            update_vibration();
            update_current();
            update_dht();
            draw_menu(smoothed_temperature,smoothed_humidity,smoothed_current,smoothed_vibration);
        }

        if(alarm_distance()){
            deploy_break();
            deployed_breaks = 100;
            motor_status = false;
            change_color(U32_RED);
            play_alarm(21,400);
        }
        else{
            motor_status = true;
            change_color(U32_GREEN);
            deploy_break();
            stop_alarm(21);
        }

        sleep_ms(30);
        if(send_data){
            if(SAFEPREDICT_DEBUG_MODE) printf("\nSent: Type=%d, Value=%s\n", data_to_send.type, data_to_send.value);
            thing_send(data_to_send);
            send_data=false;
        }
    }

    wifi_end();
    return 0;
}