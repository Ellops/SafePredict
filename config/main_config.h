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


#define TIME_TO_UPDATE_DATA 1000

#define MOTOR_PIN 8

#define BUZZ_PIN 21
#define FREQ_BUZZER 400

#define DHT_WINDOW_SIZE 10
#define CURRENT_WINDOW_SIZE 10


const int CURRENT_PIN = 28;
const int ADC_CHANNEL_2 = 2;

float temperature_buffer[DHT_WINDOW_SIZE] = {0};
int temperature_index_window = 0;
float humidity_buffer[DHT_WINDOW_SIZE] = {0};
int humidity_index_window = 0;
float current_buffer[CURRENT_WINDOW_SIZE] = {0};
int current_index_window = 0;

float smoothed_temperature = 0.0;
float smoothed_humidity = 0.0;
float smoothed_current = 0.0;

