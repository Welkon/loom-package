#include "fir.h"

void fir_init(fir_t *filter, const int16_t *taps, int16_t *history,
              uint16_t length, uint8_t shift)
{
    filter->taps = taps;
    filter->history = history;
    filter->length = length;
    filter->index = 0;
    filter->shift = shift > 15U ? 15U : shift;
    fir_reset(filter);
}

int32_t fir_step(fir_t *filter, int16_t sample)
{
    int64_t accumulator = 0;
    uint16_t k;
    uint16_t position;
    if (filter->taps == 0 || filter->history == 0 || filter->length == 0U) {
        return 0;
    }
    filter->history[filter->index] = sample;
    for (k = 0; k < filter->length; k++) {
        position = (uint16_t)((filter->index + filter->length - k) % filter->length);
        accumulator += (int64_t)filter->taps[k] * filter->history[position];
    }
    filter->index = (uint16_t)((filter->index + 1U) % filter->length);
    return (int32_t)(accumulator / ((int64_t)1 << filter->shift));
}

void fir_reset(fir_t *filter)
{
    uint16_t i;
    filter->index = 0;
    if (filter->history != 0) {
        for (i = 0; i < filter->length; i++) {
            filter->history[i] = 0;
        }
    }
}
