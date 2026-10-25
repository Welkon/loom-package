#ifndef ONEWIRE_PORT_H
#define ONEWIRE_PORT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *context;
    uint8_t (*reset)(void *context);
    void (*write_bit)(void *context, uint8_t bit);
    uint8_t (*read_bit)(void *context);
    void (*delay_ms)(void *context, uint16_t ms);
} onewire_port_t;

#ifdef __cplusplus
}
#endif

#endif
