#ifndef MPU6050_H
#define MPU6050_H

#include "pico/stdlib.h"

extern float smoothed_vibration;

bool init_mpu6050();
void mpu6050_reset();
void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t *temp);
void update_vibration();

#endif
 