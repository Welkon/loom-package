#include <stdio.h>
#include "weighted_average.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

int main(void)
{
    const uint16_t samples[3] = {10, 20, 30};
    const uint8_t weights[3] = {1, 2, 3};
    const uint8_t zero[2] = {0, 0};

    CHECK(weighted_mean_u16(samples, weights, 3) == 23);
    CHECK(weighted_mean_u16(samples, zero, 2) == 0);
    CHECK(weighted_mean_u16(samples, weights, 0) == 0);
    CHECK(weighted_mean_u16(0, weights, 3) == 0);

    if (failures) return 1;
    puts("weighted_average: ok");
    return 0;
}
