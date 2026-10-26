#ifndef SDCARD_H
#define SDCARD_H

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
} sdcard_port_t;

typedef struct {
    sdcard_port_t port;
    uint8_t initialized;
} sdcard_t;

uint8_t sdcard_init(sdcard_t *card, sdcard_port_t port);
uint8_t sdcard_read_block(sdcard_t *card, uint32_t block, uint8_t data[512]);
uint8_t sdcard_write_block(sdcard_t *card, uint32_t block, const uint8_t data[512]);

#ifdef __cplusplus
}
#endif

#endif
