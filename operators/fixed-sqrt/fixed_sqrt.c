#include "fixed_sqrt.h"

int32_t fixed_sqrt_q16(int32_t value)
{
    uint64_t number;
    uint64_t result = 0;
    uint64_t bit = (uint64_t)1 << 62;

    if (value <= 0) {
        return 0;
    }
    number = (uint64_t)value << 16;
    while (bit > number) {
        bit >>= 2;
    }
    while (bit != 0U) {
        if (number >= result + bit) {
            number -= result + bit;
            result = (result >> 1) + bit;
        } else {
            result >>= 1;
        }
        bit >>= 2;
    }
    return (int32_t)result;
}
