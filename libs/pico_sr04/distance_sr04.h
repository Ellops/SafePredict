 #ifndef DISTANCE_SR04_H
 #define DISTANCE_SR04_H
 
 #include "pico/stdlib.h"
 /*
    Criando funções simples e intuitivas para o sensor de distancia
*/
bool init_sr04(); //Inicialações necessárias ao sensor
float measure_distance();//Função que mede a distância
bool alarm_distance();//Função que alerta caso um limite seja atingido pela distância

 #endif
 