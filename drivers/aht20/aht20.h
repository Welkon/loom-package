#ifndef AHT20_H
#define AHT20_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *context;
    uint8_t (*write)(void *context, uint8_t address, const uint8_t *data, uint16_t length);
    uint8_t (*read)(void *context, uint8_t address, uint8_t *data, uint16_t length);
    void (*delay_ms)(void *context, uint16_t milliseconds);
} aht20_port_t;

typedef struct {
    aht20_port_t port;
    uint8_t i2c_address;
} aht20_t;

uint8_t aht20_init(aht20_t *device, aht20_port_t port, uint8_t i2c_address);
uint8_t aht20_read(aht20_t *device, int32_t *milli_celsius, int32_t *milli_percent);

#ifdef __cplusplus
}
#endif

#endif
