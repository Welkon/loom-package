#ifndef UART_PORT_H
#define UART_PORT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *context;
    void (*write)(void *context, uint8_t byte);
    uint8_t (*read_ready)(void *context);
    uint8_t (*read)(void *context);
    uint32_t (*now_us)(void *context);
} uart_port_t;

#ifdef __cplusplus
}
#endif

#endif
