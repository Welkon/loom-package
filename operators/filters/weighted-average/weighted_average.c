#include "weighted_average.h"

uint16_t weighted_mean_u16(const uint16_t *samples, const uint8_t *weights, uint8_t count)
{
    uint32_t products = 0;
    uint32_t weight_sum = 0;
    uint8_t index;

    if (samples == 0 || weights == 0 || count == 0U) {
        return 0;
    }
    for (index = 0; index < count; index++) {
        products += (uint32_t)samples[index] * weights[index];
        weight_sum += weights[index];
    }
    if (weight_sum == 0U) {
        return 0;
    }
    return (uint16_t)((products + weight_sum / 2U) / weight_sum);
}
