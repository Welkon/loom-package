#include "hysteresis.h"

void hysteresis_init(hysteresis_t *filter, int32_t low, int32_t high, uint8_t initial)
{
    filter->low = low;
    filter->high = high;
    filter->state = initial ? 1U : 0U;
}

uint8_t hysteresis_step(hysteresis_t *filter, int32_t value)
{
    if (!filter->state && value >= filter->high) {
        filter->state = 1;
    } else if (filter->state && value <= filter->low) {
        filter->state = 0;
    }
    return filter->state;
}
