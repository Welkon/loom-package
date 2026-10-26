#include <stdio.h>
#include <string.h>
#include "adxl345.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

typedef struct {
    uint8_t regs[64];
    uint16_t writes;
    uint16_t delays;
} fake_t;

static uint8_t fake_write(void *context, uint8_t address, uint8_t reg,
                          const uint8_t *data, uint16_t length)
{
    fake_t *fake = (fake_t *)context;
    (void)address;
    for (uint16_t i = 0; i < length; i++) fake->regs[reg + i] = data[i];
    fake->writes++;
    return 1;
}

static uint8_t fake_read(void *context, uint8_t address, uint8_t reg,
                         uint8_t *data, uint16_t length)
{
    fake_t *fake = (fake_t *)context;
    (void)address;
    for (uint16_t i = 0; i < length; i++) data[i] = fake->regs[reg + i];
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
    adxl345_t accel;
    adxl345_port_t port;
    adxl345_reading_t reading;

    memset(&fake, 0, sizeof(fake));
    port.context = &fake;
    port.write = fake_write;
    port.read = fake_read;
    port.delay_ms = fake_delay;
    fake.regs[0x00] = 0xE5;
    CHECK(adxl345_init(&accel, port, 0x53) == 1);
    CHECK(fake.regs[0x2C] == 0x0A && fake.regs[0x31] == 0x0B && fake.regs[0x2D] == 0x08);
    CHECK(fake.delays == 1);

    fake.regs[0x32] = 0x00; fake.regs[0x33] = 0x40;
    fake.regs[0x34] = 0x00; fake.regs[0x35] = 0xC0;
    fake.regs[0x36] = 0x00; fake.regs[0x37] = 0x00;
    CHECK(adxl345_read(&accel, &reading) == 1);
    CHECK(reading.x == 16384 && reading.y == -16384 && reading.z == 0);
    CHECK(reading.x_mg == 63897 && reading.y_mg == -63897 && reading.z_mg == 0);

    fake.regs[0x00] = 0x00;
    CHECK(adxl345_init(&accel, port, 0x53) == 0);

    if (failures) return 1;
    puts("adxl345: ok");
    return 0;
}
