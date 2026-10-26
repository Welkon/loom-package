#ifndef HX711_H
#define HX711_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *context;
    uint8_t (*read_dout)(void *context);
    void (*set_sck)(void *context, uint8_t level);
    void (*delay_us)(void *context, uint16_t microseconds);
} hx711_port_t;

typedef struct {
    hx711_port_t port;
    uint8_t gain;
} hx711_t;

uint8_t hx711_init(hx711_t *device, hx711_port_t port);
uint8_t hx711_ready(hx711_t *device);
uint8_t hx711_read(hx711_t *device, int32_t *value);

#ifdef __cplusplus
}
#endif

#endif
