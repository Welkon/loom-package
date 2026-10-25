#include "clamp_filter.h"

void clamp_filter_init(clamp_filter_t *filter, int32_t initial, uint16_t max_delta)
{
    filter->value = initial;
    filter->max_delta = max_delta;
}

int32_t clamp_filter_step(clamp_filter_t *filter, int32_t sample)
{
    int64_t delta = (int64_t)sample - filter->value;
    int64_t limit = (int64_t)filter->max_delta;

    if (delta > limit) {
        delta = limit;
    } else if (delta < -limit) {
        delta = -limit;
    }
    filter->value = (int32_t)((int64_t)filter->value + delta);
    return filter->value;
}

void clamp_filter_reset(clamp_filter_t *filter, int32_t value)
{
    filter->value = value;
}
