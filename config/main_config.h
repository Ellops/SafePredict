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