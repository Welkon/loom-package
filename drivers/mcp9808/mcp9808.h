#ifndef MCP9808_H
#define MCP9808_H

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
} mcp9808_port_t;

typedef struct {
    mcp9808_port_t port;
    uint8_t i2c_address;
} mcp9808_t;

uint8_t mcp9808_init(mcp9808_t *device, mcp9808_port_t port, uint8_t i2c_address);
uint8_t mcp9808_read(mcp9808_t *device, int32_t *milli_celsius);

#ifdef __cplusplus
}
#endif

#endif
