#include "debounce.h"

void debounce_init(debounce_t *filter, uint16_t threshold_ms)
{
    filter->threshold_ms = threshold_ms;
    filter->elapsed_ms = 0;
    filter->candidate = 0;
    filter->stable = 0;
}

uint8_t debounce_step(debounce_t *filter, uint8_t raw, uint16_t elapsed_ms)
{
    raw = raw ? 1U : 0U;
    if (raw != filter->stable) {
        if (raw != filter->candidate) {
            filter->candidate = raw;
            filter->elapsed_ms = 0;
        }
        filter->elapsed_ms = (uint16_t)(filter->elapsed_ms + elapsed_ms);
        if (filter->elapsed_ms >= filter->threshold_ms) {
            filter->stable = raw;
            filter->elapsed_ms = 0;
        }
    } else {
        filter->candidate = filter->stable;
        filter->elapsed_ms = 0;
    }
    return filter->stable;
}
