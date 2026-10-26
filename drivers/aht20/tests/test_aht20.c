#include <stdio.h>
#include <string.h>
#include "aht20.h"

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
    uint8_t status;
    uint8_t data[7];
    uint8_t command[3];
    uint16_t delays;
} fake_t;

static uint8_t fake_write(void *context, uint8_t address, const uint8_t *data, uint16_t length)
{
    fake_t *fake = (fake_t *)context;
    (void)address;
    if (length != 3) return 0;
    memcpy(fake->command, data, 3);
    return 1;
}

static uint8_t fake_read(void *context, uint8_t address, uint8_t *data, uint16_t length)
{
    fake_t *fake = (fake_t *)context;
    (void)address;
    if (length == 1) {
        data[0] = fake->status;
    } else if (length == 7) {
        memcpy(data, fake->data, 7);
    } else {
        return 0;
    }
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
    aht20_t sensor;
    aht20_port_t port;
    int32_t temperature, humidity;

    memset(&fake, 0, sizeof(fake));
    port.context = &fake;
    port.write = fake_write;
    port.read = fake_read;
    port.delay_ms = fake_delay;
    fake.status = 0x18;
    CHECK(aht20_init(&sensor, port, 0x38) == 1);
    CHECK(fake.delays == 1);

    fake.data[0] = 0x18;
    fake.data[1] = 0x80; fake.data[2] = 0x00; fake.data[3] = 0x06;
    fake.data[4] = 0x00; fake.data[5] = 0x00;
    fake.data[6] = crc8(fake.data, 6);
    CHECK(aht20_read(&sensor, &temperature, &humidity) == 1);
    CHECK(fake.command[0] == 0xAC && fake.command[1] == 0x33 && fake.command[2] == 0x00);
    CHECK(temperature == 25000 && humidity == 50000);

    fake.data[6] ^= 1;
    CHECK(aht20_read(&sensor, &temperature, &humidity) == 0);
    fake.data[6] ^= 1;
    fake.data[0] = 0x98;
    CHECK(aht20_read(&sensor, &temperature, &humidity) == 0);

    fake.status = 0x00;
    CHECK(aht20_init(&sensor, port, 0x38) == 0);
    CHECK(fake.command[0] == 0xBE && fake.command[1] == 0x08);

    if (failures) return 1;
    puts("aht20: ok");
    return 0;
}
