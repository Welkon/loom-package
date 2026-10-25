#include "median_filter.h"

void median_filter_init(median_filter_t *filter, uint16_t *window, uint8_t size)
{
    filter->window = window;
    filter->size = size;
    filter->count = 0;
    filter->index = 0;
}

uint16_t median_filter_step(median_filter_t *filter, uint16_t sample)
{
    uint8_t target;
    uint8_t i;

    if (filter->size == 0) {
        return 0;
    }

    filter->window[filter->index] = sample;
    filter->index = (uint8_t)((filter->index + 1U) % filter->size);
    if (filter->count < filter->size) {
        filter->count++;
    }

    target = (uint8_t)((filter->count - 1U) / 2U);
    for (i = 0; i < filter->count; i++) {
        uint16_t value = filter->window[i];
        uint8_t less = 0;
        uint8_t equal = 0;
        uint8_t j;

        for (j = 0; j < filter->count; j++) {
            uint16_t other = filter->window[j];
            if (other < value) {
                less++;
            } else if (other == value) {
                equal++;
            }
        }
        if (less <= target && (uint8_t)(less + equal) > target) {
            return value;
        }
    }
    return sample;
}

void median_filter_reset(median_filter_t *filter)
{
    filter->count = 0;
    filter->index = 0;
}
