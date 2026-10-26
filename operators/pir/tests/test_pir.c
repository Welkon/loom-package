#include <stdio.h>
#include "pir.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } \
} while (0)

static void feed(pir_t *pir, uint16_t value, uint32_t *now, uint8_t batches)
{
    for (uint8_t b = 0; b < batches; b++) {
        for (uint8_t i = 0; i < PIR_BATCH_SAMPLES; i++) {
            CHECK(pir_step(pir, value, (*now)++) == 0);
        }
    }
}

int main(void)
{
    pir_t pir;
    uint32_t now = 0;
    uint8_t event = 0;

    pir_init(&pir, 245);
    pir_set_warm(&pir, 1);
    feed(&pir, 1000, &now, 16);
    feed(&pir, 1000, &now, 2);

    for (uint16_t value = 1000; value <= 1500; value += 100) {
        for (uint8_t i = 0; i < PIR_BATCH_SAMPLES; i++) {
            if (pir_step(&pir, value, now++)) event = 1;
        }
    }
    CHECK(event == 1);

    pir_init(&pir, 245);
    pir_set_warm(&pir, 1);
    feed(&pir, 1500, &now, 18);
    event = 0;
    for (uint16_t value = 1500; value >= 1000; value -= 100) {
        for (uint8_t i = 0; i < PIR_BATCH_SAMPLES; i++) {
            if (pir_step(&pir, value, now++)) event = 1;
        }
    }
    CHECK(event == 1);

    pir_init(&pir, 1000);
    pir_set_warm(&pir, 1);
    now = 0;
    feed(&pir, 1000, &now, 18);
    event = 0;
    for (uint16_t value = 1000; value <= 1400; value += 100) {
        for (uint8_t i = 0; i < PIR_BATCH_SAMPLES; i++) {
            if (pir_step(&pir, value, now++)) event = 1;
        }
    }
    CHECK(event == 0);

    pir_init(&pir, 245);
    pir_set_warm(&pir, 1);
    now = 0;
    feed(&pir, 1000, &now, 18);
    CHECK(pir_step(&pir, 3000, now++) == 0);
    CHECK(pir_step(&pir, 5000, now++) == 0);
    feed(&pir, 5000, &now, 6);
    event = 0;
    for (uint16_t value = 5000; value <= 5600; value += 100) {
        for (uint8_t i = 0; i < PIR_BATCH_SAMPLES; i++) {
            if (pir_step(&pir, value, now++)) event = 1;
        }
    }
    CHECK(event == 0);
    now += PIR_ERROR_LOCK_MS;
    event = 0;
    for (uint16_t value = 5600; value <= 6200; value += 100) {
        for (uint8_t i = 0; i < PIR_BATCH_SAMPLES; i++) {
            if (pir_step(&pir, value, now++)) event = 1;
        }
    }
    CHECK(event == 1);

    if (failures) return 1;
    puts("pir: ok");
    return 0;
}
