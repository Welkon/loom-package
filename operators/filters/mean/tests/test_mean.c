#include <stdio.h>
#include "mean.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

int main(void)
{
    const uint16_t four[4] = {10, 20, 30, 40};
    const uint16_t two[2] = {1, 2};
    const uint16_t high[2] = {65535, 65535};

    CHECK(mean_u16(four, 4) == 25);
    CHECK(mean_u16(two, 2) == 2);
    CHECK(mean_u16(high, 2) == 65535);
    CHECK(mean_u16(two, 0) == 0);
    CHECK(mean_u16(0, 4) == 0);

    if (failures) return 1;
    puts("mean: ok");
    return 0;
}
