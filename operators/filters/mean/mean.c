#include "mean.h"

uint16_t mean_u16(const uint16_t *samples, uint8_t count)
{
    uint32_t sum = 0;
    uint8_t index;

    if (samples == 0 || count == 0U) {
        return 0;
    }
    for (index = 0; index < count; index++) {
        sum += samples[index];
    }
    return (uint16_t)((sum + (uint16_t)(count / 2U)) / count);
}
