#include <stdio.h>
#include <string.h>
#include "mcp4725.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

typedef struct {
    uint8_t data[4];
    uint16_t length;
} fake_t;

static uint8_t fake_write(void *context, uint8_t address, const uint8_t *data, uint16_t length)
{
    fake_t *fake = (fake_t *)context;
    (void)address;
    fake->length = length;
    for (uint16_t i = 0; i < length && i < sizeof(fake->data); i++) fake->data[i] = data[i];
    return 1;
}

int main(void)
{
    fake_t fake;
    mcp4725_t dac;
    mcp4725_port_t port;

    memset(&fake, 0, sizeof(fake));
    port.context = &fake;
    port.write = fake_write;
    CHECK(mcp4725_init(&dac, port, 0x60) == 1);

    CHECK(mcp4725_write_value(&dac, 0xABC) == 1);
    CHECK(fake.length == 2 && fake.data[0] == 0x0A && fake.data[1] == 0xBC);
    CHECK(mcp4725_write_value(&dac, 0) == 1);
    CHECK(fake.data[0] == 0x00 && fake.data[1] == 0x00);
    CHECK(mcp4725_write_value(&dac, 4095) == 1);
    CHECK(fake.data[0] == 0x0F && fake.data[1] == 0xFF);
    CHECK(mcp4725_write_value(&dac, 4096) == 0);

    if (failures) return 1;
    puts("mcp4725: ok");
    return 0;
}
