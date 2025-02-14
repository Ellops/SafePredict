#include "distance_sr04.h"

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/time.h"

#define TRIG_PIN 16 //Pino que enviará o sinal
#define ECHO_PIN 18 //Pino que recebe a resposta que poderemos medir o tempo

#define TRIGGER_SCALE 12 //Distância em centímetros que alarmará

bool init_sr04(){
    /*
        Inicia os pinos nas posições corretas
    */
    gpio_init(TRIG_PIN);
    gpio_init(ECHO_PIN);

    gpio_set_dir(TRIG_PIN, GPIO_OUT);
    gpio_set_dir(ECHO_PIN, GPIO_IN);
    return true;
}

/*
    Calculando a velocidade do som se mede a distância de algum obstáculo
*/
float measure_distance() {
    gpio_put(TRIG_PIN, 1);
    sleep_us(10);
    gpio_put(TRIG_PIN, 0);

    absolute_time_t start_wait = get_absolute_time();
    while (!gpio_get(ECHO_PIN)) {
        if (absolute_time_diff_us(start_wait, get_absolute_time()) > 1000000) {
            return -1;
        }
    }

    absolute_time_t start_time = get_absolute_time();
    while (gpio_get(ECHO_PIN)) {
        if (absolute_time_diff_us(start_time, get_absolute_time()) > 26100) {
            return 1000;
        }
    }
    absolute_time_t end_time = get_absolute_time();

    int64_t pulse_length = absolute_time_diff_us(start_time, end_time);

    float distance_cm = pulse_length * 0.0343 / 2; //Metade da velocidade do som pelo tamanho do pulso

    return distance_cm;
}

bool alarm_distance(){
    if(measure_distance()<TRIGGER_SCALE){
        return true;
    }
    return false;
}