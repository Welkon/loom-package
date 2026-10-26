#include <stdio.h>
#include <string.h>
#include "ina219.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

typedef struct {
    uint16_t config;
    uint16_t calibration;
    uint16_t bus;
    uint16_t shunt;
    uint16_t current;
    uint16_t power;
} fake_t;

static uint8_t fake_write(void *context, uint8_t address, uint8_t reg,
                          const uint8_t *data, uint16_t length)
{
    fake_t *fake = (fake_t *)context;
    uint16_t value = (uint16_t)(((uint16_t)data[0] << 8) | data[1]);
    (void)address; (void)length;
    if (reg == 0x00) fake->config = value;
    if (reg == 0x05) fake->calibration = value;
    return 1;
}

static uint8_t fake_read(void *context, uint8_t address, uint8_t reg,
                         uint8_t *data, uint16_t length)
{
    fake_t *fake = (fake_t *)context;
    uint16_t value = 0;
    (void)address; (void)length;
    if (reg == 0x01) value = fake->shunt;
    if (reg == 0x02) value = fake->bus;
    if (reg == 0x03) value = fake->power;
    if (reg == 0x04) value = fake->current;
    data[0] = (uint8_t)(value >> 8);
    data[1] = (uint8_t)value;
    return 1;
}

int main(void)
{
    fake_t fake;
    ina219_t monitor;
    ina219_port_t port;
    uint32_t value;
    int32_t signed_value;

    memset(&fake, 0, sizeof(fake));
    port.context = &fake;
    port.write = fake_write;
    port.read = fake_read;
    CHECK(ina219_init(&monitor, port, 0x40, 100, 100) == 1);
    CHECK(fake.config == 0x399F && fake.calibration == 4096);

    fake.bus = 0x1900;
    CHECK(ina219_read_bus_voltage_mv(&monitor, &value) == 1 && value == 3200);
    fake.shunt = 0x0010;
    CHECK(ina219_read_shunt_voltage_uv(&monitor, &signed_value) == 1 && signed_value == 160);
    fake.current = 0x0064;
    CHECK(ina219_read_current_ua(&monitor, &signed_value) == 1 && signed_value == 10000);
    fake.power = 0x0100;
    CHECK(ina219_read_power_mw(&monitor, &value) == 1 && value == 512);

    CHECK(ina219_init(&monitor, port, 0x40, 0, 100) == 0);
    CHECK(ina219_init(&monitor, port, 0x40, 100, 0) == 0);

    if (failures) return 1;
    puts("ina219: ok");
    return 0;
}
