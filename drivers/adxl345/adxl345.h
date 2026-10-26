#ifndef ADXL345_H
#define ADXL345_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *context;
    uint8_t (*write)(void *context, uint8_t address, uint8_t reg,
                     const uint8_t *data, uint16_t length);
    uint8_t (*read)(void *context, uint8_t address, uint8_t reg,
                    uint8_t *data, uint16_t length);
    void (*delay_ms)(void *context, uint16_t milliseconds);
} adxl345_port_t;

typedef struct {
    adxl345_port_t port;
    uint8_t i2c_address;
} adxl345_t;

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
    int32_t x_mg;
    int32_t y_mg;
    int32_t z_mg;
} adxl345_reading_t;

uint8_t adxl345_init(adxl345_t *device, adxl345_port_t port, uint8_t i2c_address);
uint8_t adxl345_read(adxl345_t *device, adxl345_reading_t *reading);

#ifdef __cplusplus
}
#endif

#endif
