#include <stdio.h>
#include "moving_average.h"

static int failures = 0;

#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

int main(void)
{
    uint16_t window[4];
    moving_average_t filter;

    moving_average_init(&filter, window, 4);
    CHECK(moving_average_step(&filter, 10) == 10);
    CHECK(moving_average_step(&filter, 20) == 15);
    CHECK(moving_average_step(&filter, 30) == 20);
    CHECK(moving_average_step(&filter, 40) == 25);
    CHECK(moving_average_step(&filter, 50) == 35);
    CHECK(moving_average_step(&filter, 60) == 45);

    moving_average_reset(&filter);
    CHECK(moving_average_step(&filter, 5) == 5);

    moving_average_init(&filter, window, 1);
    CHECK(moving_average_step(&filter, 123) == 123);
    CHECK(moving_average_step(&filter, 456) == 456);

    moving_average_init(&filter, window, 2);
    CHECK(moving_average_step(&filter, 1) == 1);
    CHECK(moving_average_step(&filter, 2) == 2);

    moving_average_init(&filter, window, 0);
    CHECK(moving_average_step(&filter, 9) == 0);

    if (failures != 0) {
        return 1;
    }
    puts("moving_average: ok");
    return 0;
}
