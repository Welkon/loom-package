#ifndef MOVING_AVERAGE_H
#define MOVING_AVERAGE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t *window;
    uint8_t size;
    uint8_t count;
    uint8_t index;
    uint32_t sum;
} moving_average_t;

void moving_average_init(moving_average_t *filter, uint16_t *window, uint8_t size);
uint16_t moving_average_step(moving_average_t *filter, uint16_t sample);
void moving_average_reset(moving_average_t *filter);

#ifdef __cplusplus
}
#endif

#endif
