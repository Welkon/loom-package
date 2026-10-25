#include "biquad.h"

void biquad_init(biquad_t *filter, int32_t b0, int32_t b1, int32_t b2,
                 int32_t a1, int32_t a2, uint8_t shift)
{
    filter->b0 = b0;
    filter->b1 = b1;
    filter->b2 = b2;
    filter->a1 = a1;
    filter->a2 = a2;
    filter->x1 = 0;
    filter->x2 = 0;
    filter->y1 = 0;
    filter->y2 = 0;
    filter->shift = shift > 15U ? 15U : shift;
}

int32_t biquad_step(biquad_t *filter, int32_t sample)
{
    int64_t accumulator = (int64_t)filter->b0 * sample
        + (int64_t)filter->b1 * filter->x1
        + (int64_t)filter->b2 * filter->x2
        - (int64_t)filter->a1 * filter->y1
        - (int64_t)filter->a2 * filter->y2;
    int32_t output = (int32_t)(accumulator / ((int64_t)1 << filter->shift));
    filter->x2 = filter->x1;
    filter->x1 = sample;
    filter->y2 = filter->y1;
    filter->y1 = output;
    return output;
}

void biquad_reset(biquad_t *filter)
{
    filter->x1 = 0;
    filter->x2 = 0;
    filter->y1 = 0;
    filter->y2 = 0;
}
