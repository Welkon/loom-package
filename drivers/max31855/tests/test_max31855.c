#include <stdio.h>
#include "max31855.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

typedef struct {
    uint32_t frame;
    uint8_t index;
} fake_t;

static void fake_select(void *context)
{
    fake_t *fake = (fake_t *)context;
    fake->index = 0;
}
static uint8_t fake_transfer(void *context, uint8_t value)
{
    fake_t *fake = (fake_t *)context;
    uint8_t shift = (uint8_t)(24U - 8U * fake->index);
    (void)value;
    fake->index++;
    return (uint8_t)(fake->frame >> shift);
}
static void fake_deselect(void *context) { (void)context; }

int main(void)
{
    fake_t fake;
    max31855_reading_t reading;
    max31855_port_t port;

    fake.index = 0;
    port.context = &fake;
    port.select = fake_select;
    port.transfer = fake_transfer;
    port.deselect = fake_deselect;

    fake.frame = ((uint32_t)(101 & 0x3FFF) << 18) | ((uint32_t)(392 & 0x0FFF) << 4);
    CHECK(max31855_read(port, &reading) == 1);
    CHECK(reading.thermocouple_milli_c == 25250);
    CHECK(reading.internal_milli_c == 24500);
    CHECK(reading.fault == 0);

    fake.frame = ((uint32_t)((-10 & 0x3FFF)) << 18) | ((uint32_t)((-4 & 0x0FFF)) << 4);
    CHECK(max31855_read(port, &reading) == 1);
    CHECK(reading.thermocouple_milli_c == -2500);
    CHECK(reading.internal_milli_c == -250);

    fake.frame = (1UL << 16) | 0x05;
    CHECK(max31855_read(port, &reading) == 1);
    CHECK(reading.fault == 1 && reading.open_circuit == 1);
    CHECK(reading.short_to_vcc == 1 && reading.short_to_gnd == 0);

    if (failures) return 1;
    puts("max31855: ok");
    return 0;
}
