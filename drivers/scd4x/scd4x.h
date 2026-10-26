#ifndef SCD4X_H
#define SCD4X_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *context;
    uint8_t (*write)(void *context, uint8_t address, const uint8_t *data, uint16_t length);
    uint8_t (*read)(void *context, uint8_t address, uint8_t *data, uint16_t length);
    void (*delay_ms)(void *context, uint16_t milliseconds);
} scd4x_port_t;

typedef struct {
    scd4x_port_t port;
    uint8_t i2c_address;
} scd4x_t;

uint8_t scd4x_init(scd4x_t *sensor, scd4x_port_t port, uint8_t i2c_address);
uint8_t scd4x_read(scd4x_t *sensor, uint16_t *co2_ppm,
                   int32_t *milli_celsius, uint32_t *milli_percent);

#ifdef __cplusplus
}
#endif

#endif
