#include <stdio.h>
#include "clamp_debounce.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

int main(void)
{
    clamp_debounce_t filter;

    clamp_debounce_init(&filter, 1000, 10, 3);
    CHECK(clamp_debounce_step(&filter, 1050) == 1000);
    CHECK(clamp_debounce_step(&filter, 1050) == 1000);
    CHECK(clamp_debounce_step(&filter, 1050) == 1010);
    CHECK(clamp_debounce_step(&filter, 1000) == 1010);
    CHECK(clamp_debounce_step(&filter, 1000) == 1010);
    CHECK(clamp_debounce_step(&filter, 1000) == 1000);
    CHECK(clamp_debounce_step(&filter, 1000) == 1000);

    clamp_debounce_init(&filter, 0, 5, 0);
    CHECK(clamp_debounce_step(&filter, 100) == 5);

    if (failures) return 1;
    puts("clamp_debounce: ok");
    return 0;
}
