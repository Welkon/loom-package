#include <stdio.h>
#include "clamp_average.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

int main(void)
{
    uint16_t window[2];
    clamp_average_t filter;

    clamp_average_init(&filter, 0, 1000, window, 2);
    CHECK(clamp_average_step(&filter, 10) == 10);
    CHECK(clamp_average_step(&filter, 20) == 15);
    CHECK(clamp_average_step(&filter, 30) == 25);
    CHECK(clamp_average_step(&filter, 40) == 35);

    clamp_average_init(&filter, 100, 10, window, 1);
    CHECK(clamp_average_step(&filter, 1000) == 110);
    CHECK(clamp_average_step(&filter, 0) == 100);

    clamp_average_init(&filter, 0, 10, window, 0);
    CHECK(clamp_average_step(&filter, 100) == 0);

    if (failures) return 1;
    puts("clamp_average: ok");
    return 0;
}
