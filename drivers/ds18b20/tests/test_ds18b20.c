#include <stdio.h>
#include <string.h>
#include "ds18b20.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

typedef struct {
    uint8_t scratchpad[9];
    uint8_t bit_index;
    uint8_t present;
    uint8_t commands[4];
    uint8_t command_count;
    uint16_t delay_ms;
} fake_t;

static uint8_t fake_reset(void *context)
{
    return ((fake_t *)context)->present;
}

static void fake_write_bit(void *context, uint8_t bit)
{
    fake_t *fake = (fake_t *)context;
    (void)bit;
    if (fake->command_count == 0U) fake->command_count = 0U;
}

static uint8_t fake_read_bit(void *context)
{
    fake_t *fake = (fake_t *)context;
    uint8_t bit = (uint8_t)((fake->scratchpad[fake->bit_index / 8U] >> (fake->bit_index % 8U)) & 1U);
    fake->bit_index++;
    return bit;
}

static void fake_delay(void *context, uint16_t ms)
{
    ((fake_t *)context)->delay_ms = ms;
}

static onewire_port_t fake_port(fake_t *fake)
{
    onewire_port_t port;
    port.context = fake;
    port.reset = fake_reset;
    port.write_bit = fake_write_bit;
    port.read_bit = fake_read_bit;
    port.delay_ms = fake_delay;
    return port;
}

int main(void)
{
    fake_t fake;
    int16_t temperature = 0;

    memset(&fake, 0, sizeof(fake));
    fake.present = 1;
    fake.scratchpad[0] = 0x91;
    fake.scratchpad[1] = 0x01;
    fake.scratchpad[2] = 0x4B;
    fake.scratchpad[3] = 0x46;
    fake.scratchpad[4] = 0x7F;
    fake.scratchpad[5] = 0xFF;
    fake.scratchpad[6] = 0x0C;
    fake.scratchpad[7] = 0x10;
    fake.scratchpad[8] = 0x70;
    CHECK(ds18b20_read(fake_port(&fake), &temperature) == 1);
    CHECK(temperature == 0x0191);
    CHECK(fake.delay_ms == 750);

    fake.present = 0;
    CHECK(ds18b20_read(fake_port(&fake), &temperature) == 0);

    fake.present = 1;
    fake.scratchpad[8] = 0x00;
    CHECK(ds18b20_read(fake_port(&fake), &temperature) == 0);

    if (failures) return 1;
    puts("ds18b20: ok");
    return 0;
}
