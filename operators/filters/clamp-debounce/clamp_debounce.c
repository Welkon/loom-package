#include "clamp_debounce.h"

void clamp_debounce_init(clamp_debounce_t *filter, uint16_t initial, uint16_t max_delta, uint16_t threshold)
{
    filter->value = initial;
    filter->max_delta = max_delta;
    filter->candidate = initial;
    filter->threshold = threshold;
    filter->count = 0;
}

uint16_t clamp_debounce_step(clamp_debounce_t *filter, uint16_t sample)
{
    int32_t delta = (int32_t)sample - filter->value;
    uint16_t clamped;

    if (delta > (int32_t)filter->max_delta) {
        delta = (int32_t)filter->max_delta;
    } else if (delta < -(int32_t)filter->max_delta) {
        delta = -(int32_t)filter->max_delta;
    }
    clamped = (uint16_t)((int32_t)filter->value + delta);

    if (clamped == filter->value) {
        filter->candidate = filter->value;
        filter->count = 0;
    } else if (clamped == filter->candidate) {
        if (filter->count < UINT16_MAX) {
            filter->count++;
        }
    } else {
        filter->candidate = clamped;
        filter->count = 1;
    }
    if (filter->threshold == 0U || filter->count >= filter->threshold) {
        filter->value = filter->candidate;
        filter->count = 0;
    }
    return filter->value;
}
