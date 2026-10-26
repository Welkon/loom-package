#ifndef MCP4725_H
#define MCP4725_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *context;
    uint8_t (*write)(void *context, uint8_t address, const uint8_t *data, uint16_t length);
} mcp4725_port_t;

typedef struct {
    mcp4725_port_t port;
    uint8_t i2c_address;
} mcp4725_t;

uint8_t mcp4725_init(mcp4725_t *device, mcp4725_port_t port, uint8_t i2c_address);
uint8_t mcp4725_write_value(mcp4725_t *device, uint16_t value);

#ifdef __cplusplus
}
#endif

#endif
