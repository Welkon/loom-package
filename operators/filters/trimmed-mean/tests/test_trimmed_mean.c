#include <stdio.h>
#include "trimmed_mean.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

int main(void)
{
    const uint16_t pir[6] = {10, 100, 20, 90, 30, 40};
    const uint16_t three[3] = {1, 100, 3};
    const uint16_t duplicates[4] = {5, 5, 9, 9};
    const uint16_t two[2] = {1, 2};

    CHECK(trimmed_mean(pir, 6) == 45);
    CHECK(trimmed_mean(three, 3) == 3);
    CHECK(trimmed_mean(duplicates, 4) == 7);
    CHECK(trimmed_mean(two, 2) == 0);
    CHECK(trimmed_mean(0, 6) == 0);

    if (failures) return 1;
    puts("trimmed_mean: ok");
    return 0;
}
