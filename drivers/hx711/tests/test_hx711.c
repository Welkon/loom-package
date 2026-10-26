#include <stdio.h>
#include <string.h>
#include "hx711.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

typedef struct {
    uint32_t bits;
    uint8_t bit_count;
    uint8_t sck_high;
    uint8_t sck_low;
    uint16_t delays;
    uint8_t not_ready;
    uint8_t started;
} fake_t;

static uint8_t fake_dout(void *context)
{
    fake_t *fake = (fake_t *)context;
    if (fake->not_ready) return 1;
    if (!fake->started || fake->bit_count >= 24) return 0;
    return (uint8_t)((fake->bits >> (23 - fake->bit_count)) & 1U);
}
static void fake_sck(void *context, uint8_t level)
{
    fake_t *fake = (fake_t *)context;
    if (level) {
        fake->sck_high++;
        fake->started = 1;
    } else {
        fake->sck_low++;
        if (fake->bit_count < 24) fake->bit_count++;
    }
}
static void fake_delay(void *context, uint16_t us)
{
    fake_t *fake = (fake_t *)context;
    (void)us;
    fake->delays++;
}

int main(void)
{
    fake_t fake;
    hx711_t scale;
    hx711_port_t port;
    int32_t value;

    memset(&fake, 0, sizeof(fake));
    port.context = &fake;
    port.read_dout = fake_dout;
    port.set_sck = fake_sck;
    port.delay_us = fake_delay;
    CHECK(hx711_init(&scale, port) == 1);
    CHECK(scale.gain == 128);

    fake.bits = 0x123456;
    fake.bit_count = 0;
    CHECK(hx711_read(&scale, &value) == 1);
    CHECK(value == 0x123456);
    CHECK(fake.sck_high == 25);

    memset(&fake, 0, sizeof(fake));
    fake.bits = 0xFFFFFE;
    fake.bit_count = 0;
    CHECK(hx711_read(&scale, &value) == 1);
    CHECK(value == -2);

    fake.bit_count = 0;
    fake.sck_high = 0;
    fake.started = 0;
    scale.gain = 64;
    CHECK(hx711_read(&scale, &value) == 1);
    CHECK(fake.sck_high == 27);

    fake.not_ready = 1;
    CHECK(hx711_read(&scale, &value) == 0);

    if (failures) return 1;
    puts("hx711: ok");
    return 0;
}
