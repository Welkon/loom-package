#ifndef EMA_FILTER_H
#define EMA_FILTER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int32_t value;
    uint8_t shift;
} ema_filter_t;

void ema_filter_init(ema_filter_t *filter, int32_t initial, uint8_t shift);
int32_t ema_filter_step(ema_filter_t *filter, int32_t sample);
void ema_filter_reset(ema_filter_t *filter, int32_t value);

#ifdef __cplusplus
}
#endif

#endif
