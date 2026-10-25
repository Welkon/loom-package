#ifndef MEDIAN_FILTER_H
#define MEDIAN_FILTER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t *window;
    uint8_t size;
    uint8_t count;
    uint8_t index;
} median_filter_t;

void median_filter_init(median_filter_t *filter, uint16_t *window, uint8_t size);
uint16_t median_filter_step(median_filter_t *filter, uint16_t sample);
void median_filter_reset(median_filter_t *filter);

#ifdef __cplusplus
}
#endif

#endif
