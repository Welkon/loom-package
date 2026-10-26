#ifndef MCP23017_H
#define MCP23017_H

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
} mcp23017_port_t;

typedef struct {
    mcp23017_port_t port;
    uint8_t i2c_address;
} mcp23017_t;

uint8_t mcp23017_init(mcp23017_t *device, mcp23017_port_t port, uint8_t i2c_address);
uint8_t mcp23017_pin_mode(mcp23017_t *device, uint8_t pin, uint8_t output);
uint8_t mcp23017_write_pin(mcp23017_t *device, uint8_t pin, uint8_t value);
uint8_t mcp23017_read_pin(mcp23017_t *device, uint8_t pin, uint8_t *value);
uint8_t mcp23017_write_port(mcp23017_t *device, uint8_t port, uint8_t value);
uint8_t mcp23017_read_port(mcp23017_t *device, uint8_t port, uint8_t *value);
uint8_t mcp23017_set_pullup(mcp23017_t *device, uint8_t pin, uint8_t enable);

#ifdef __cplusplus
}
#endif

#endif
