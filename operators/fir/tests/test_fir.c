#include <stdio.h>
#include "fir.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

int main(void)
{
    static const int16_t average3[3] = {10923, 10923, 10923};
    static const int16_t average2[2] = {16384, 16384};
    static const int16_t gain[1] = {32767};
    int16_t history[3];
    fir_t filter;

    fir_init(&filter, average2, history, 2, 15);
    CHECK(fir_step(&filter, 100) == 50);
    CHECK(fir_step(&filter, 200) == 150);
    CHECK(fir_step(&filter, 300) == 250);

    fir_init(&filter, average3, history, 3, 15);
    CHECK(fir_step(&filter, 30) == 10);
    CHECK(fir_step(&filter, 60) == 30);
    CHECK(fir_step(&filter, 90) == 60);
    fir_reset(&filter);
    CHECK(history[0] == 0 && history[1] == 0 && history[2] == 0);

    fir_init(&filter, gain, history, 3, 15);
    CHECK(fir_step(&filter, 11) == 10);
    fir_init(&filter, NULL, history, 3, 15);
    CHECK(fir_step(&filter, 11) == 0);

    if (failures) return 1;
    puts("fir: ok");
    return 0;
}
