#include <stdio.h>
#include "gamma_lut.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

int main(void)
{
    const uint16_t table[3] = {0, 1000, 2000};
    const uint16_t one[1] = {42};

    CHECK(gamma_lut_apply(table, 3, 0, 100) == 0);
    CHECK(gamma_lut_apply(table, 3, 25, 100) == 500);
    CHECK(gamma_lut_apply(table, 3, 50, 100) == 1000);
    CHECK(gamma_lut_apply(table, 3, 100, 100) == 2000);
    CHECK(gamma_lut_apply(table, 3, 200, 100) == 2000);
    CHECK(gamma_lut_apply(one, 1, 50, 100) == 42);
    CHECK(gamma_lut_apply(0, 3, 50, 100) == 0);
    CHECK(gamma_lut_apply(table, 0, 50, 100) == 0);
    CHECK(gamma_lut_apply(table, 3, 50, 0) == 0);

    if (failures) return 1;
    puts("gamma_lut: ok");
    return 0;
}
