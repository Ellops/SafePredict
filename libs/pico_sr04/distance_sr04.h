 #ifndef DISTANCE_SR04_H
 #define DISTANCE_SR04_H
 
 #include "pico/stdlib.h"
 
bool init_sr04();
float measure_distance();
bool alarm_distance();

 #endif
 