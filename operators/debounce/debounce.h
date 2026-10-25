#ifndef DEBOUNCE_H
#define DEBOUNCE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t threshold_ms;
    uint16_t elapsed_ms;
    uint8_t candidate;
    uint8_t stable;
} debounce_t;

void debounce_init(debounce_t *filter, uint16_t threshold_ms);
uint8_t debounce_step(debounce_t *filter, uint8_t raw, uint16_t elapsed_ms);

#ifdef __cplusplus
}
#endif

#endif
