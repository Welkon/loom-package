#include "gamma_lut.h"

uint16_t gamma_lut_apply(const uint16_t *table, uint8_t entries,
                         uint16_t input, uint16_t input_max)
{
    uint32_t position;
    uint8_t index;
    uint32_t fraction;
    uint64_t lower;
    uint64_t upper;

    if (table == 0 || entries == 0U || input_max == 0U) {
        return 0;
    }
    if (entries == 1U || input >= input_max) {
        return table[entries - 1U];
    }
    position = (uint32_t)input * (uint32_t)(entries - 1U);
    index = (uint8_t)(position / input_max);
    fraction = position % input_max;
    lower = (uint64_t)table[index] * (input_max - fraction);
    upper = (uint64_t)table[index + 1U] * fraction;
    return (uint16_t)((lower + upper + input_max / 2U) / input_max);
}
