#include "ema_filter.h"

static uint8_t clamp_shift(uint8_t shift)
{
    return shift > 15U ? 15U : shift;
}

void ema_filter_init(ema_filter_t *filter, int32_t initial, uint8_t shift)
{
    filter->value = initial;
    filter->shift = clamp_shift(shift);
}

int32_t ema_filter_step(ema_filter_t *filter, int32_t sample)
{
    int32_t factor = (int32_t)(1UL << filter->shift);
    int64_t delta = (int64_t)sample - filter->value;

    filter->value = (int32_t)((int64_t)filter->value + delta / factor);
    return filter->value;
}

void ema_filter_reset(ema_filter_t *filter, int32_t value)
{
    filter->value = value;
}
