#include <stdio.h>
#include <string.h>
#include "scd4x.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } \
} while (0)

static uint8_t crc8(const uint8_t *data, uint16_t length)
{
    uint8_t crc = 0xFF;
    for (uint16_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (uint8_t bit = 0; bit < 8; bit++) crc = (uint8_t)((crc & 0x80) ? ((crc << 1) ^ 0x31) : (crc << 1));
    }
    return crc;
}

typedef struct {
    uint8_t command[2];
    uint8_t data[9];
    uint16_t commands;
    uint16_t delays;
} fake_t;

static uint8_t fake_write(void *context, uint8_t address, const uint8_t *data, uint16_t length)
{
    fake_t *fake = (fake_t *)context;
    (void)address;
    if (length == 2) {
        fake->command[0] = data[0];
        fake->command[1] = data[1];
        fake->commands++;
        return 1;
    }
    return 0;
}

static uint8_t fake_read(void *context, uint8_t address, uint8_t *data, uint16_t length)
{
    fake_t *fake = (fake_t *)context;
    (void)address;
    if (length == 3) {
        data[0] = 0x00; data[1] = 0x01; data[2] = crc8(data, 2);
        return 1;
    }
    if (length == 9) {
        memcpy(data, fake->data, 9);
        return 1;
    }
    return 0;
}

static void fake_delay(void *context, uint16_t ms)
{
    fake_t *fake = (fake_t *)context;
    (void)ms;
    fake->delays++;
}

int main(void)
{
    fake_t fake;
    scd4x_t sensor;
    scd4x_port_t port;
    uint16_t co2;
    int32_t temperature;
    uint32_t humidity;

    memset(&fake, 0, sizeof(fake));
    port.context = &fake;
    port.write = fake_write;
    port.read = fake_read;
    port.delay_ms = fake_delay;
    CHECK(scd4x_init(&sensor, port, 0x62) == 1);
    CHECK(fake.command[0] == 0x21 && fake.command[1] == 0xB1);

    fake.data[0] = 0x03; fake.data[1] = 0x20; fake.data[2] = crc8(fake.data, 2);
    fake.data[3] = 0x66; fake.data[4] = 0x66; fake.data[5] = crc8(&fake.data[3], 2);
    fake.data[6] = 0x80; fake.data[7] = 0x00; fake.data[8] = crc8(&fake.data[6], 2);
    CHECK(scd4x_read(&sensor, &co2, &temperature, &humidity) == 1);
    CHECK(co2 == 800);
    CHECK(temperature == 24998);
    CHECK(humidity == 50000);
    CHECK(fake.command[0] == 0xEC && fake.command[1] == 0x05);

    fake.data[8] ^= 1;
    CHECK(scd4x_read(&sensor, &co2, &temperature, &humidity) == 0);

    if (failures) return 1;
    puts("scd4x: ok");
    return 0;
}
