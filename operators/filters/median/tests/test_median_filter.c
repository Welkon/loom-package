#include <stdio.h>
#include "median_filter.h"

static int failures = 0;

#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

int main(void)
{
    uint16_t window[5];
    median_filter_t filter;

    median_filter_init(&filter, window, 5);
    CHECK(median_filter_step(&filter, 10) == 10);
    CHECK(median_filter_step(&filter, 100) == 10);
    CHECK(median_filter_step(&filter, 20) == 20);
    CHECK(median_filter_step(&filter, 90) == 20);
    CHECK(median_filter_step(&filter, 30) == 30);
    CHECK(median_filter_step(&filter, 1000) == 90);

    median_filter_reset(&filter);
    CHECK(median_filter_step(&filter, 7) == 7);

    median_filter_init(&filter, window, 1);
    CHECK(median_filter_step(&filter, 123) == 123);
    CHECK(median_filter_step(&filter, 456) == 456);

    median_filter_init(&filter, window, 0);
    CHECK(median_filter_step(&filter, 9) == 0);

    if (failures != 0) {
        return 1;
    }
    puts("median_filter: ok");
    return 0;
}
