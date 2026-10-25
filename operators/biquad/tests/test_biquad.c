#include <stdio.h>
#include "biquad.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

int main(void)
{
    biquad_t filter;

    biquad_init(&filter, 1 << 15, 0, 0, 0, 0, 15);
    CHECK(biquad_step(&filter, 123) == 123);
    CHECK(biquad_step(&filter, -45) == -45);

    biquad_init(&filter, 10923, 10923, 10923, 0, 0, 15);
    CHECK(biquad_step(&filter, 30) == 10);
    CHECK(biquad_step(&filter, 60) == 30);
    CHECK(biquad_step(&filter, 90) == 60);
    biquad_reset(&filter);
    CHECK(filter.x1 == 0 && filter.y1 == 0);

    biquad_init(&filter, 1 << 15, 0, 0, 0, 0, 20);
    CHECK(filter.shift == 15);

    if (failures) return 1;
    puts("biquad: ok");
    return 0;
}
