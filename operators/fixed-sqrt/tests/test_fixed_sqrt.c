#include <stdio.h>
#include "fixed_sqrt.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

int main(void)
{
    CHECK(fixed_sqrt_q16(4 << 16) == 2 << 16);
    CHECK(fixed_sqrt_q16(100 << 16) == 10 << 16);
    CHECK(fixed_sqrt_q16(1 << 16) == 1 << 16);
    CHECK(fixed_sqrt_q16(0) == 0);
    CHECK(fixed_sqrt_q16(-1) == 0);
    {
        int32_t root = fixed_sqrt_q16(2 << 16);
        CHECK(root >= 92680 && root <= 92682);
    }

    if (failures) return 1;
    puts("fixed_sqrt: ok");
    return 0;
}
