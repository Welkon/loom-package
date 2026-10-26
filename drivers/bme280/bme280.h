#ifndef BME280_H
#define BME280_H

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
} bme280_port_t;

typedef struct {
    bme280_port_t port;
    uint8_t i2c_address;
    uint16_t t1;
    int16_t t2;
    int16_t t3;
    uint16_t p1;
    int16_t p2;
    int16_t p3;
    int16_t p4;
    int16_t p5;
    int16_t p6;
    int16_t p7;
    int16_t p8;
    int16_t p9;
    uint8_t h1;
    int16_t h2;
    uint8_t h3;
    int16_t h4;
    int16_t h5;
    int8_t h6;
    int32_t t_fine;
} bme280_t;

typedef struct {
    int32_t temperature_milli_c;
    uint32_t pressure_pa;
    uint32_t humidity_milli_percent;
} bme280_reading_t;

uint8_t bme280_init(bme280_t *device, bme280_port_t port, uint8_t i2c_address);
uint8_t bme280_read(bme280_t *device, bme280_reading_t *reading);

#ifdef __cplusplus
}
#endif

#endif
