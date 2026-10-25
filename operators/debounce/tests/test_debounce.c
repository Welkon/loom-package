#include <stdio.h>
#include "debounce.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

int main(void)
{
    debounce_t filter;

    debounce_init(&filter, 20);
    CHECK(debounce_step(&filter, 1, 10) == 0);
    CHECK(debounce_step(&filter, 1, 9) == 0);
    CHECK(debounce_step(&filter, 1, 1) == 1);
    CHECK(debounce_step(&filter, 0, 19) == 1);
    CHECK(debounce_step(&filter, 0, 1) == 0);

    debounce_init(&filter, 20);
    CHECK(debounce_step(&filter, 1, 10) == 0);
    CHECK(debounce_step(&filter, 0, 10) == 0);
    CHECK(debounce_step(&filter, 1, 20) == 1);

    debounce_init(&filter, 0);
    CHECK(debounce_step(&filter, 1, 0) == 1);
    CHECK(debounce_step(&filter, 0, 0) == 0);

    if (failures) return 1;
    puts("debounce: ok");
    return 0;
}
