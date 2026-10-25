#ifndef CLAMP_FILTER_H
#define CLAMP_FILTER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int32_t value;
    uint16_t max_delta;
} clamp_filter_t;

void clamp_filter_init(clamp_filter_t *filter, int32_t initial, uint16_t max_delta);
int32_t clamp_filter_step(clamp_filter_t *filter, int32_t sample);
void clamp_filter_reset(clamp_filter_t *filter, int32_t value);

#ifdef __cplusplus
}
#endif

#endif
