#ifndef WS2812_H
#define WS2812_H

#include "port.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    ws2812_port_t port;
} ws2812_t;

void ws2812_init(ws2812_t *strip, ws2812_port_t port);
void ws2812_send_rgb(ws2812_t *strip, const uint8_t *rgb, uint16_t count);
void ws2812_send_grb(ws2812_t *strip, const uint8_t *grb, uint16_t count);

#ifdef __cplusplus
}
#endif

#endif
