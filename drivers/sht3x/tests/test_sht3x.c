#include <stdio.h>
#include <string.h>
#include "sht3x.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

static uint8_t crc8(const uint8_t *data, uint16_t length)
{
    uint8_t crc = 0xFF;
    for (uint16_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (uint8_t bit = 0; bit < 8; bit++)
            if ((crc & 0x80U) != 0U) {
                crc = (uint8_t)((crc << 1) ^ 0x31U);
            } else {
                crc = (uint8_t)(crc << 1);
            }
    }
    return crc;
}

typedef struct {
    uint8_t command[2];
    uint8_t data[6];
    uint16_t delays;
} fake_t;

static uint8_t fake_write(void *context, uint8_t address, const uint8_t *data, uint16_t length)
{
    fake_t *fake = (fake_t *)context;
    (void)address;
    if (length != 2) return 0;
    fake->command[0] = data[0];
    fake->command[1] = data[1];
    return 1;
}

static uint8_t fake_read(void *context, uint8_t address, uint8_t *data, uint16_t length)
{
    fake_t *fake = (fake_t *)context;
    (void)address;
    if (length != 6) return 0;
    memcpy(data, fake->data, 6);
    return 1;
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
    sht3x_t sensor;
    sht3x_port_t port;
    int32_t temperature, humidity;

    memset(&fake, 0, sizeof(fake));
    port.context = &fake;
    port.write = fake_write;
    port.read = fake_read;
    port.delay_ms = fake_delay;
    CHECK(sht3x_init(&sensor, port, 0x44) == 1);
    CHECK(fake.command[0] == 0x30 && fake.command[1] == 0xA2);

    fake.data[0] = 0x66; fake.data[1] = 0x66; fake.data[2] = crc8(fake.data, 2);
    fake.data[3] = 0x80; fake.data[4] = 0x00; fake.data[5] = crc8(&fake.data[3], 2);
    CHECK(sht3x_read(&sensor, SHT3X_HIGH, &temperature, &humidity) == 1);
    CHECK(fake.command[0] == 0x24 && fake.command[1] == 0x00);
    CHECK(temperature == 25000 && humidity == 50000);

    CHECK(sht3x_read(&sensor, SHT3X_MEDIUM, &temperature, &humidity) == 1);
    CHECK(fake.command[1] == 0x0B);
    CHECK(sht3x_read(&sensor, SHT3X_LOW, &temperature, &humidity) == 1);
    CHECK(fake.command[1] == 0x16);

    fake.data[2] ^= 1;
    CHECK(sht3x_read(&sensor, SHT3X_HIGH, &temperature, &humidity) == 0);

    if (failures) return 1;
    puts("sht3x: ok");
    return 0;
}
