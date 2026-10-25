#ifndef CLAMP_DEBOUNCE_H
#define CLAMP_DEBOUNCE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t value;
    uint16_t max_delta;
    uint16_t candidate;
    uint16_t threshold;
    uint16_t count;
} clamp_debounce_t;

void clamp_debounce_init(clamp_debounce_t *filter, uint16_t initial, uint16_t max_delta, uint16_t threshold);
uint16_t clamp_debounce_step(clamp_debounce_t *filter, uint16_t sample);

#ifdef __cplusplus
}
#endif

#endif
