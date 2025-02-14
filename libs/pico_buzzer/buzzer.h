#ifndef BUZZER_H
#define BUZZER_H

#include "pico/stdlib.h"

void init_buzzer();
void play_alarm(uint pin, uint frequency);
void stop_alarm(uint pin);

#endif
 