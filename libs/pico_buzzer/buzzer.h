#ifndef BUZZER_H
#define BUZZER_H

#include "pico/stdlib.h"

void init_buzzer();
void play_tone(uint pin, uint frequency, uint duration_ms);

#endif
 