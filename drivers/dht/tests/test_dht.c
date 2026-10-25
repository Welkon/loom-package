#include <stdio.h>
#include <string.h>
#include "dht.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

static void frame(const uint8_t bytes[5], uint16_t *highs)
{
    uint8_t i;
    for (i = 0; i < 40U; i++) {
        highs[i] = (bytes[i / 8U] & (uint8_t)(1U << (7U - (i % 8U)))) ? 70U : 26U;
    }
}

int main(void)
{
    uint16_t highs[40];
    uint8_t bytes[5];
    dht11_reading_t d11;
    dht22_reading_t d22;

    bytes[0] = 55; bytes[1] = 0; bytes[2] = 24; bytes[3] = 0; bytes[4] = 79;
    frame(bytes, highs);
    CHECK(dht11_decode(highs, 40, &d11) == 1);
    CHECK(d11.humidity == 55 && d11.temperature == 24);

    bytes[0] = 0x02; bytes[1] = 0x8D; bytes[2] = 0x80; bytes[3] = 0x7C;
    bytes[4] = (uint8_t)(0x02 + 0x8D + 0x80 + 0x7C);
    frame(bytes, highs);
    CHECK(dht22_decode(highs, 40, &d22) == 1);
    CHECK(d22.humidity_tenths == 653 && d22.temperature_tenths == -124);

    bytes[4] ^= 0xFFU;
    frame(bytes, highs);
    CHECK(dht22_decode(highs, 40, &d22) == 0);
    CHECK(dht11_decode(highs, 39, &d11) == 0);
    CHECK(dht11_decode(NULL, 40, &d11) == 0);

    if (failures) return 1;
    puts("dht: ok");
    return 0;
}
