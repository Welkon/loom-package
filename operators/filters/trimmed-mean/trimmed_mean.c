#include "trimmed_mean.h"

uint16_t trimmed_mean(const uint16_t *samples, uint8_t count)
{
    uint16_t minimum;
    uint16_t maximum;
    uint32_t sum = 0;
    uint8_t index;

    if (samples == 0 || count < 3U) {
        return 0;
    }

    minimum = samples[0];
    maximum = samples[0];
    for (index = 0; index < count; index++) {
        uint16_t sample = samples[index];
        sum += sample;
        if (sample < minimum) {
            minimum = sample;
        }
        if (sample > maximum) {
            maximum = sample;
        }
    }
    sum -= minimum;
    sum -= maximum;
    return (uint16_t)((sum + (uint16_t)(count - 2U) / 2U) / (uint16_t)(count - 2U));
}
