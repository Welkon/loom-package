#ifndef PCF8574_H
#define PCF8574_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *context;
    uint8_t (*write)(void *context, uint8_t address, const uint8_t *data, uint16_t length);
    uint8_t (*read)(void *context, uint8_t address, uint8_t *data, uint16_t length);
} pcf8574_port_t;

typedef struct {
    pcf8574_port_t port;
    uint8_t i2c_address;
    uint8_t output;
} pcf8574_t;

uint8_t pcf8574_init(pcf8574_t *device, pcf8574_port_t port, uint8_t i2c_address);
uint8_t pcf8574_write_port(pcf8574_t *device, uint8_t value);
uint8_t pcf8574_read_port(pcf8574_t *device, uint8_t *value);
uint8_t pcf8574_write_pin(pcf8574_t *device, uint8_t pin, uint8_t value);
uint8_t pcf8574_read_pin(pcf8574_t *device, uint8_t pin, uint8_t *value);

#ifdef __cplusplus
}
#endif

#endif
