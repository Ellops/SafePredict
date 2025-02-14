/*
    Definição de Flags de controle
        -Onlybitdog ativa o modo que não precisa de perifericos externos a placa
        -Debug Mode ativa various outputs uteis
*/

#ifndef SAFEPREDICT_ONLY_BITDOG_MODE
    #define SAFEPREDICT_ONLY_BITDOG_MODE 0
#endif

#define SAFEPREDICT_DEBUG_MODE 1

/* 
    Definiçao para o display OLED
*/

#define I2C_SDA_PIN 14 //Pino de Data do i2c
#define I2C_SCL_PIN 15 //Pino de clock do i2c

#define OLED_WIDTH 128 //Comprimento do OLED
#define OLED_HEIGHT 32 //Altura do OLED

#define FONT_WIDTH 5 //Comprimento da fonte utilizada
#define FONT_HEIGHT 8 //Altura da fonte utilizada
#define SPACING 1 //Espaçamento utilizado na escrita
#define MAX_CHARS_PER_LINE (OLED_WIDTH / (FONT_WIDTH + SPACING)) //Máximo de caracteres em uma linha
#define MAX_LINES (OLED_HEIGHT / FONT_HEIGHT) //Máximo de linhas no display

/* 
    Macro para centralizar as mensagens
*/
#define GET_CENTER_LINE(text_length) \
    ((OLED_WIDTH - (text_length) * ((FONT_WIDTH) + (SPACING))) / 2)


/*
    Definição da velocidade de remarcação interna dos sensores.
*/
#define TIME_TO_UPDATE_DATA 1000

/*
    Pino do freio, no exemplo utilizado como freio motor
*/
#define MOTOR_PIN 8

/*
    Pino do buzzer e frequência do sinal transmitido
*/
#define BUZZ_PIN 21
#define FREQ_BUZZER 400

/*
    Dados para filtro de média móvel para reduzir erros das leituras
*/
#define DHT_WINDOW_SIZE 10
#define CURRENT_WINDOW_SIZE 10

float temperature_buffer[DHT_WINDOW_SIZE] = {0};
int temperature_index_window = 0;
float humidity_buffer[DHT_WINDOW_SIZE] = {0};
int humidity_index_window = 0;
float current_buffer[CURRENT_WINDOW_SIZE] = {0};
int current_index_window = 0;

/*
    Definições do sensor de corrente utilizando o ADC_2
*/
const int CURRENT_PIN = 28;
const int ADC_CHANNEL_2 = 2;

/*
    Variáveis globais filtradas
*/
float smoothed_temperature = 0.0; //Temperatura Imediações motor
float smoothed_humidity = 0.0; //Umidade Imediações motor
float smoothed_current = 0.0; //Corrente sendo utilizada pelo motor

