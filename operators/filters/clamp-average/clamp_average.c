#include "clamp_average.h"

void clamp_average_init(clamp_average_t *filter, uint16_t initial, uint16_t max_delta, uint16_t *window, uint8_t size)
{
    filter->value = initial;
    filter->max_delta = max_delta;
    filter->window = window;
    filter->size = size;
    filter->count = 0;
    filter->index = 0;
    filter->sum = 0;
}

uint16_t clamp_average_step(clamp_average_t *filter, uint16_t sample)
{
    int32_t delta;

    if (filter->size == 0U) {
        return filter->value;
    }
    delta = (int32_t)sample - filter->value;
    if (delta > (int32_t)filter->max_delta) {
        delta = (int32_t)filter->max_delta;
    } else if (delta < -(int32_t)filter->max_delta) {
        delta = -(int32_t)filter->max_delta;
    }
    filter->value = (uint16_t)((int32_t)filter->value + delta);

    if (filter->count < filter->size) {
        filter->window[filter->index] = filter->value;
        filter->sum += filter->value;
        filter->count++;
        filter->index++;
        if (filter->index == filter->size) {
            filter->index = 0;
        }
    } else {
        filter->sum -= filter->window[filter->index];
        filter->window[filter->index] = filter->value;
        filter->sum += filter->value;
        filter->index++;
        if (filter->index == filter->size) {
            filter->index = 0;
        }
    }
    return (uint16_t)((filter->sum + (uint16_t)(filter->count / 2U)) / filter->count);
}
