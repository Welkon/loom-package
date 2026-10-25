#ifndef MPU6050_H
#define MPU6050_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *context;
    uint8_t (*write)(void *context, uint8_t address, const uint8_t *data, uint16_t length);
    uint8_t (*read)(void *context, uint8_t address, uint8_t *data, uint16_t length);
    void (*delay_ms)(void *context, uint16_t milliseconds);
} mpu6050_port_t;

typedef struct {
    mpu6050_port_t port;
    uint8_t i2c_address;
} mpu6050_t;

typedef struct {
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
    int16_t temperature;
} mpu6050_raw_t;

typedef struct {
    int32_t accel_x_mg;
    int32_t accel_y_mg;
    int32_t accel_z_mg;
    int32_t gyro_x_mdps;
    int32_t gyro_y_mdps;
    int32_t gyro_z_mdps;
    int32_t temperature_milli_celsius;
} mpu6050_scaled_t;

uint8_t mpu6050_init(mpu6050_t *device, mpu6050_port_t port, uint8_t i2c_address);
uint8_t mpu6050_read_raw(mpu6050_t *device, mpu6050_raw_t *raw);
uint8_t mpu6050_read_scaled(mpu6050_t *device, mpu6050_scaled_t *scaled);

#ifdef __cplusplus
}
#endif

#endif
