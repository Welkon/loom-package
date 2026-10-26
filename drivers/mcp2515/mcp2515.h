#ifndef MCP2515_H
#define MCP2515_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *context;
    void (*select)(void *context);
    uint8_t (*transfer)(void *context, uint8_t value);
    void (*deselect)(void *context);
    void (*delay_ms)(void *context, uint16_t milliseconds);
} mcp2515_port_t;

typedef struct {
    uint32_t id;
    uint8_t extended;
    uint8_t dlc;
    uint8_t data[8];
} can_frame_t;

typedef struct {
    mcp2515_port_t port;
    uint8_t clock_mhz;
    uint16_t bitrate_kbps;
} mcp2515_t;

uint8_t mcp2515_init(mcp2515_t *device, mcp2515_port_t port,
                     uint8_t clock_mhz, uint16_t bitrate_kbps);
uint8_t mcp2515_send(mcp2515_t *device, const can_frame_t *frame);
uint8_t mcp2515_receive(mcp2515_t *device, can_frame_t *frame);

#ifdef __cplusplus
}
#endif

#endif
