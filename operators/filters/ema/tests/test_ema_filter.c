#include <stdio.h>
#include "ema_filter.h"

static int failures = 0;

#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

int main(void)
{
    ema_filter_t filter;

    ema_filter_init(&filter, 0, 2);
    CHECK(ema_filter_step(&filter, 100) == 25);
    CHECK(ema_filter_step(&filter, 100) == 43);
    CHECK(ema_filter_step(&filter, 0) == 33);
    CHECK(ema_filter_step(&filter, 0) == 25);

    ema_filter_reset(&filter, 0);
    CHECK(ema_filter_step(&filter, 100) == 25);

    ema_filter_init(&filter, 7, 0);
    CHECK(ema_filter_step(&filter, 100) == 100);

    ema_filter_init(&filter, 0, 1);
    CHECK(ema_filter_step(&filter, -100) == -50);
    CHECK(ema_filter_step(&filter, -100) == -75);

    ema_filter_init(&filter, 0, 20);
    CHECK(ema_filter_step(&filter, 100) == 0);

    if (failures != 0) {
        return 1;
    }
    puts("ema_filter: ok");
    return 0;
}
