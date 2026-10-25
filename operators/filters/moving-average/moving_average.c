#include "moving_average.h"

void moving_average_init(moving_average_t *filter, uint16_t *window, uint8_t size)
{
    filter->window = window;
    filter->size = size;
    filter->count = 0;
    filter->index = 0;
    filter->sum = 0;
}

uint16_t moving_average_step(moving_average_t *filter, uint16_t sample)
{
    uint32_t rounded;

    if (filter->size == 0) {
        return 0;
    }

    if (filter->count < filter->size) {
        filter->window[filter->index] = sample;
        filter->sum += sample;
        filter->count++;
        filter->index++;
        if (filter->index == filter->size) {
            filter->index = 0;
        }
    } else {
        filter->sum -= filter->window[filter->index];
        filter->window[filter->index] = sample;
        filter->sum += sample;
        filter->index++;
        if (filter->index == filter->size) {
            filter->index = 0;
        }
    }

    rounded = filter->sum + (uint32_t)(filter->count / 2U);
    return (uint16_t)(rounded / filter->count);
}

void moving_average_reset(moving_average_t *filter)
{
    filter->count = 0;
    filter->index = 0;
    filter->sum = 0;
}
