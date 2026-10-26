#include <stdio.h>
#include <string.h>
#include "mcp9808.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

typedef struct {
    uint8_t temperature[2];
    uint8_t config[2];
} fake_t;

static uint8_t fake_write(void *context, uint8_t address, uint8_t reg,
                          const uint8_t *data, uint16_t length)
{
    fake_t *fake = (fake_t *)context;
    (void)address;
    if (reg == 0x01 && length == 2) {
        fake->config[0] = data[0];
        fake->config[1] = data[1];
        return 1;
    }
    return 0;
}

static uint8_t fake_read(void *context, uint8_t address, uint8_t reg,
                         uint8_t *data, uint16_t length)
{
    fake_t *fake = (fake_t *)context;
    (void)address;
    if (reg == 0x05 && length == 2) {
        data[0] = fake->temperature[0];
        data[1] = fake->temperature[1];
        return 1;
    }
    return 0;
}

int main(void)
{
    fake_t fake;
    mcp9808_t sensor;
    mcp9808_port_t port;
    int32_t temperature;

    memset(&fake, 0, sizeof(fake));
    port.context = &fake;
    port.write = fake_write;
    port.read = fake_read;
    CHECK(mcp9808_init(&sensor, port, 0x18) == 1);
    CHECK(fake.config[0] == 0x00 && fake.config[1] == 0x03);

    fake.temperature[0] = 0x01; fake.temperature[1] = 0x91; /* 25.0625 C */
    CHECK(mcp9808_read(&sensor, &temperature) == 1 && temperature == 25062);

    fake.temperature[0] = 0x1F; fake.temperature[1] = 0xFF; /* -0.0625 C */
    CHECK(mcp9808_read(&sensor, &temperature) == 1 && temperature == -62);

    fake.temperature[0] = 0x00; fake.temperature[1] = 0x00;
    CHECK(mcp9808_read(&sensor, &temperature) == 1 && temperature == 0);

    if (failures) return 1;
    puts("mcp9808: ok");
    return 0;
}
