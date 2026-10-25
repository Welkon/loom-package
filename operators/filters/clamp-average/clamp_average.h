#ifndef CLAMP_AVERAGE_H
#define CLAMP_AVERAGE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t value;
    uint16_t max_delta;
    uint16_t *window;
    uint8_t size;
    uint8_t count;
    uint8_t index;
    uint32_t sum;
} clamp_average_t;

void clamp_average_init(clamp_average_t *filter, uint16_t initial, uint16_t max_delta, uint16_t *window, uint8_t size);
uint16_t clamp_average_step(clamp_average_t *filter, uint16_t sample);

#ifdef __cplusplus
}
#endif

#endif
