#include <stdio.h>
#include "clamp_filter.h"

static int failures = 0;

#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

int main(void)
{
    clamp_filter_t filter;

    clamp_filter_init(&filter, 1000, 10);
    CHECK(clamp_filter_step(&filter, 1005) == 1005);
    CHECK(clamp_filter_step(&filter, 1050) == 1015);
    CHECK(clamp_filter_step(&filter, 900) == 1005);
    CHECK(clamp_filter_step(&filter, -1000) == 995);
    CHECK(clamp_filter_step(&filter, 995) == 995);

    clamp_filter_reset(&filter, 0);
    CHECK(clamp_filter_step(&filter, 100) == 10);

    clamp_filter_init(&filter, 42, 0);
    CHECK(clamp_filter_step(&filter, 1000) == 42);
    CHECK(clamp_filter_step(&filter, -1000) == 42);

    if (failures != 0) {
        return 1;
    }
    puts("clamp_filter: ok");
    return 0;
}
