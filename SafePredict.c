/*
    É necessario colocar credenciais WIFI e thingspeak no config/.env e recompilar
*/


#include "main_config.h" //Arquivo com definições referentes a este arquivo principal

/*
    Bibliotecas pertencentes ao STD do raspberry pico
*/
#include "pico/stdlib.h" //Funções básicas
#include "pico/binary_info.h" //Tratamento a dados binarios
#include "pico/util/queue.h" //Fila e movimentações com fila
#include "hardware/pwm.h" //Definições de PWM
#include "hardware/clocks.h" //Definições de timers e controle de clock
#include "hardware/i2c.h" //Definições quanto a TWI ou i2c
#include "hardware/adc.h" //Definições do conversor analógico digital

/*
    Bibliotecas pertencentes ao STD do C
*/
#include "math.h" //Operações matemáticas não elementares

/*
    Bibliotecas externas a esse projeto(Desenvolvidas inteiramente por terceiros)
*/
#include "dht.h" //@vmilea
#include "ssd1306.h" //@daschr

/*
    Bibliotecas internas a esse projeto(Desenvolvidas para o projeto inteira ou parcialmente)
*/
#include "distance_sr04.h" //Leitura do sensor ultrasônico de distância (Inteiramente Projeto)
#include "rgb_digital.h" //Iniciação e controle básicao digital do led RGB (Inteiramente Projeto)
#include "mpu6050.h" //Leitura do sensor de posição mpu6050 (Parcialmente Projeto, baseada em exemplos PICO)
#include "buzzer.h" //Controle do buzzer através de PWM (Inteiramente Projeto)
#include "http_request.h" //Construção e controle requisições http (Parcialmente Projeto, baseada em exemplo @ProfIuriSouza)
#include "wifi_manager.h" //Simplificação da conexão WIFI usando (Inteiramente Projeto)

/*
    Entidade principal do oled
*/
ssd1306_t oled; 

/*
    Função de inicialização do i2c e em seguida oled como definido pela biblioteca
*/
void init_oled() {
    i2c_init(i2c1, 100 * 1000); 
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C); 
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C); 
    gpio_pull_up(I2C_SDA_PIN); 
    gpio_pull_up(I2C_SCL_PIN);

    ssd1306_init(&oled, OLED_WIDTH, OLED_HEIGHT, 0x3C, i2c1); 
    ssd1306_clear(&oled); 
    ssd1306_show(&oled); 
} 

/*
    Uso de funções da biblioteca para demonstrar as informações do motor no oled
*/
void draw_data(float temperature, float humidity, float current, float vibration) {
    ssd1306_clear(&oled);
    int x = 0;
    int y = 0;

    char temp_str[20];
    snprintf(temp_str, sizeof(temp_str), "T: %.2fC, H: %.2f", temperature,humidity);
    ssd1306_draw_string(&oled, x, y, 1, temp_str);
    y += FONT_HEIGHT + SPACING;


    char current_str[20];
    snprintf(current_str, sizeof(current_str), "Cur: %.2f A", current);
    ssd1306_draw_string(&oled, x, y, 1, current_str);
    y += FONT_HEIGHT + SPACING;

    char vibration_str[20];
    snprintf(vibration_str, sizeof(vibration_str), "Vib: %.2f Mod", vibration);
    ssd1306_draw_string(&oled, x, y, 1, vibration_str);
    ssd1306_show(&oled);
}

/*
    Estruturas necessárias pela biblioteca de DHT
*/
const dht_model_t DHT_MODEL = DHT11;
const uint DHT_DATA_PIN = 17;
dht_t dht;
/*
    Construção das funções que geram dados e filtram com uma média móvel, existindo
    2 possibilidades atraves da flag SAFEPREDICT_ONLY_BITDOG_MODE:
        -Dados retirados de sensores reais;
        -Dados aleatórios de sensores fictícios para que ainda seja possível fazer os testes;
*/
#if SAFEPREDICT_ONLY_BITDOG_MODE
    #include <time.h>
    void update_dht() {
        float humidity_increment = ((rand() % 21) - 10) / 10.0; 
        float temperature_increment = ((rand() % 21) - 10) / 10.0;
        
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

/*
    Definições para motor
*/
bool motor_status = true; //Status para o motor na aplicação
uint deployed_breaks = 0; //Se os freios foram disparados antes do próximo envio

/*
    Função que inicia os GPIO responsáveis por freiar o motor
*/
int init_motor(){
    gpio_init(MOTOR_PIN);
    gpio_set_dir(MOTOR_PIN, GPIO_OUT);    
}

/*
    Função que realiza um freio motor
*/
void deploy_break(){
    if(motor_status){
        gpio_put(MOTOR_PIN,0);
    }
    else{
        gpio_put(MOTOR_PIN,1);
    }
}

/*
    Criando uma fila de dados para melhor organização dos envios
        queue_entry_t definido no arquivo lib/pico_http_request/http_request.h
*/
queue_t thingspeak_queue; //A fila que será iniciada
queue_entry_t data_to_send; //Qual próximo dado a ser enviado ao thingspeak e em ql canal
volatile bool send_data = false; //Flag que controla se é necessário enviar dados controlado por timers
volatile uint32_t last_data_collection = 0; //Ultima vez que novos dados foram coletados

/*
    Função que coloca dados na fila dado um tipo de dados e um valor
*/
void enqueue_data(thingspeak_type_t type, float value) {
    queue_entry_t entry;
    entry.type = type;
    snprintf(entry.value, sizeof(entry.value), "%.3f", value);
    if (!queue_try_add(&thingspeak_queue, &entry)) {
        printf("Queue is full!\n");
    }
}

/*
    Callback para o timer que coloca novos dados na fila com ações diferentes dependendo do dado
    e sempre circulando entre os dados para respeitar o limite de 15 segundos entre 1 envio ao thingspeak
*/

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

/*
    Callback para o timer que controla e prepara o próximo envio que será feito em loop
*/
bool dequeue_timer_callback(struct repeating_timer *t) {
    queue_try_remove(&thingspeak_queue, &data_to_send);
    send_data = true;
    return true;
}

int main(void){
    #if defined(SAFEPREDICT_ONLY_BITDOG_MODE)
    srand(time(NULL)); //Inicia um seed aleatório caso não tenhamos sensores presentes;
    #endif
    stdio_init_all(); //Inicia toda comunição serial

    /*
        Todas funções de inicialização dos sistemas utilizados no código
    */
    init_oled(); //Inicia o sistema do OLED
    init_buzzer(); //Inicia o sistema do buzzer
    init_RGB(); //Inicia o sistema do LED RGB
    
    init_sr04(); //Inicia o sistema do sensor ultrassônico
    init_mpu6050(); //Inicia o sistema do mpu(Acelerômetro, Giroscópio, Sensor temperatura chip)
    dht_init(&dht, DHT_MODEL, pio0, DHT_DATA_PIN, true); //Inicia o DHT(Sensor temperatura e Umidade)
    init_motor(); //Inicia o freio motor
    //Inicia o ADC para o sistema responsável por medir corrente
    adc_init(); 
    adc_gpio_init(CURRENT_PIN);

    queue_init(&thingspeak_queue, sizeof(queue_entry_t), 10); //Inicia a fila com um tamanho de 10 arquivos

    mpu6050_reset(); //Liga o MPU através de um reset nos registradores de controle

    /*
        Estruturas do SDK responsáveis por definir os timers
    */
    struct repeating_timer enqueue_timer; //Timer para colocar elementos na fila
    struct repeating_timer dequeue_timer; //Timer para retirar elementos da fila

    /*
        Criação dos timer repitiveis indefinidamente ambos respeitando os 15 seg do thingspeak
    */
    add_repeating_timer_ms(15200, enqueue_timer_callback, NULL, &enqueue_timer);  
    add_repeating_timer_ms(15500, dequeue_timer_callback, NULL, &dequeue_timer);

    wifi_start(WIFI_SSID,WIFI_PASSWORD); //Função que inicializa o WIFI e PASSWORD
    
    while (1) {
        uint32_t current_time = to_ms_since_boot(get_absolute_time()); //Controle de tempo por timer global
        
        //Forma não bloqueante de fazer update nos dados com um tempo "relaxado" próximo a TIME_TO_UPDATE_DATA
        if (current_time - last_data_collection > TIME_TO_UPDATE_DATA){
            last_data_collection = current_time;
            update_vibration(); //Faz update da vibração
            update_current(); //Faz update do valor de corrente
            update_dht(); //Faz update de Umidade e Temperatura
            //Coloca todos valores atualizados no OLED
            draw_data(smoothed_temperature,smoothed_humidity,smoothed_current,smoothed_vibration);
        }
        
        //Resposta de proteção automática
        if(alarm_distance()){
            deploy_break(); //Ativa os freios
            deployed_breaks = 100; //Adiciona Marcação par ao thingspeak
            motor_status = false; //Supõe que o motor está parado
            change_color(U32_RED); //Muda a cor do led para demonstrar perigo
            play_alarm(BUZZ_PIN,FREQ_BUZZER); //Ativa alarme Para reforçar perigo
        }
        else{
            motor_status = true; //Considera que motor voltou a funcionar
            change_color(U32_GREEN); //Muda cor do led para demonstrar que o perigo passou
            deploy_break();//Desativa os freios
            stop_alarm(BUZZ_PIN);//Para o alarme para demonstrar que está seguro
        }

        sleep_ms(30);//Pequeno delay para estabilidade
        if(send_data){
            if(SAFEPREDICT_DEBUG_MODE) printf("\nSent: Type=%d, Value=%s\n", data_to_send.type, data_to_send.value); //Debug mostrando o próximo envio
            thing_send(data_to_send); //envia por http os dados
            send_data=false; //reseta a flag de envio
        }
    }

    wifi_end(); //Caso exista alguma falha reseta wifi por segurança
    return 0;
}