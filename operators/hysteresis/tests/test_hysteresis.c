#include <stdio.h>
#include "hysteresis.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

int main(void)
{
    hysteresis_t filter;

    hysteresis_init(&filter, 10, 20, 0);
    CHECK(hysteresis_step(&filter, 19) == 0);
    CHECK(hysteresis_step(&filter, 20) == 1);
    CHECK(hysteresis_step(&filter, 15) == 1);
    CHECK(hysteresis_step(&filter, 10) == 0);
    CHECK(hysteresis_step(&filter, 11) == 0);
    CHECK(hysteresis_step(&filter, 20) == 1);

    hysteresis_init(&filter, 10, 20, 1);
    CHECK(hysteresis_step(&filter, 15) == 1);
    CHECK(hysteresis_step(&filter, 10) == 0);

    if (failures) return 1;
    puts("hysteresis: ok");
    return 0;
}
