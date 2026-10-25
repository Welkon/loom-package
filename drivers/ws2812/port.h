#ifndef WS2812_PORT_H
#define WS2812_PORT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *context;
    void (*write_byte)(void *context, uint8_t byte);
    void (*reset)(void *context);
} ws2812_port_t;

#ifdef __cplusplus
}
#endif

#endif
