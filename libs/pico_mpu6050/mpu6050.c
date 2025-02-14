#include "mpu6050.h"

#include "hardware/i2c.h"
#include "math.h"
#include "stdlib.h"

#ifndef SAFEPREDICT_ONLY_BITDOG_MODE
    #define SAFEPREDICT_ONLY_BITDOG_MODE 0
#endif

#define VIBRATION_WINDOW_SIZE 10
static const int addr = 0x68;
static float vibration_buffer[VIBRATION_WINDOW_SIZE] = {0};
static  int vibration_index_window = 0;
float smoothed_vibration = 0.0;

bool init_mpu6050(){
    i2c_init(i2c0, 400 * 1000);
    gpio_set_function(0, GPIO_FUNC_I2C);
    gpio_set_function(1, GPIO_FUNC_I2C);
    gpio_pull_up(0);
    gpio_pull_up(1);
    return true;
}

void mpu6050_reset() {

    uint8_t buf[] = {0x6B, 0x80};
    i2c_write_blocking(i2c0, addr, buf, 2, false);
    sleep_ms(100);

    buf[1] = 0x00;  
    i2c_write_blocking(i2c0, addr, buf, 2, false); 
    sleep_ms(10);
}

void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t *temp) {

    uint8_t buffer[6];


    uint8_t val = 0x3B;
    i2c_write_blocking(i2c0, addr, &val, 1, true);
    i2c_read_blocking(i2c0, addr, buffer, 6, false);

    for (int i = 0; i < 3; i++) {
        accel[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
    }


    val = 0x43;
    i2c_write_blocking(i2c0, addr, &val, 1, true);
    i2c_read_blocking(i2c0, addr, buffer, 6, false);  

    for (int i = 0; i < 3; i++) {
        gyro[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);;
    }


    val = 0x41;
    i2c_write_blocking(i2c0, addr, &val, 1, true);
    i2c_read_blocking(i2c0, addr, buffer, 2, false); 

    *temp = buffer[0] << 8 | buffer[1];
}

#if SAFEPREDICT_ONLY_BITDOG_MODE
void update_vibration() {
    #include <time.h>
    float ax = ((rand() % 21) - 10) / 10.0;  // Random increment between -1.0 and 1.0
    float ay = ((rand() % 21) - 10) / 10.0; // Random increment between -1.0 and 1.0
    float az = ((rand() % 21) - 10) / 10.0; // Random increment between -1.0 and 1.0

    float vibration = sqrt(ax * ax + ay * ay + az * az) - 1.00;

    vibration_buffer[vibration_index_window] = vibration;
    vibration_index_window = (vibration_index_window + 1) % VIBRATION_WINDOW_SIZE;

    float sum = 0;
    for (int i = 0; i < VIBRATION_WINDOW_SIZE; i++) {
        sum += vibration_buffer[i];
    }
    smoothed_vibration = sum / VIBRATION_WINDOW_SIZE;
}
#else
void update_vibration() {
    int16_t *acceleration = (int16_t *)malloc(3 * sizeof(int16_t));
    int16_t *gyro = (int16_t *)malloc(3 * sizeof(int16_t));
    if (acceleration == NULL || gyro == NULL) {
        return;
    }

    int16_t temp;
    mpu6050_read_raw(acceleration, gyro, &temp);

    float ax = (acceleration[0] + 240) / 16384.0;
    float ay = (acceleration[1] - 150) / 16384.0;
    float az = (acceleration[2] + 1876) / 16384.0;

    float vibration = sqrt(ax * ax + ay * ay + az * az) - 1.00;

    vibration_buffer[vibration_index_window] = vibration;
    vibration_index_window = (vibration_index_window + 1) % VIBRATION_WINDOW_SIZE;

    float sum = 0;
    for (int i = 0; i < VIBRATION_WINDOW_SIZE; i++) {
        sum += vibration_buffer[i];
    }
    smoothed_vibration = sum / VIBRATION_WINDOW_SIZE;

    free(acceleration);
    free(gyro);
}
#endif