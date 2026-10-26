#ifndef SHT3X_H
#define SHT3X_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *context;
    uint8_t (*write)(void *context, uint8_t address, const uint8_t *data, uint16_t length);
    uint8_t (*read)(void *context, uint8_t address, uint8_t *data, uint16_t length);
    void (*delay_ms)(void *context, uint16_t milliseconds);
} sht3x_port_t;

typedef enum {
    SHT3X_HIGH = 0,
    SHT3X_MEDIUM,
    SHT3X_LOW,
} sht3x_repeatability_t;

typedef struct {
    sht3x_port_t port;
    uint8_t i2c_address;
} sht3x_t;

uint8_t sht3x_init(sht3x_t *device, sht3x_port_t port, uint8_t i2c_address);
uint8_t sht3x_read(sht3x_t *device, sht3x_repeatability_t repeatability,
                   int32_t *milli_celsius, int32_t *milli_percent);

#ifdef __cplusplus
}
#endif

#endif
