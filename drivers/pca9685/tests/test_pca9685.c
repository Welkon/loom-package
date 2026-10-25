#include <stdio.h>
#include <string.h>
#include "pca9685.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

typedef struct {
    uint8_t regs[256];
    uint8_t pointer;
    uint16_t delays;
} fake_t;

static uint8_t fake_write(void *context, uint8_t address, const uint8_t *data, uint16_t length)
{
    fake_t *fake = (fake_t *)context;
    (void)address;
    if (length == 0) return 0;
    fake->pointer = data[0];
    for (uint16_t i = 1; i < length; i++) fake->regs[fake->pointer++] = data[i];
    return 1;
}

static uint8_t fake_read(void *context, uint8_t address, uint8_t *data, uint16_t length)
{
    fake_t *fake = (fake_t *)context;
    (void)address;
    for (uint16_t i = 0; i < length; i++) data[i] = fake->regs[fake->pointer++];
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
    pca9685_t pwm;
    pca9685_port_t port;

    memset(&fake, 0, sizeof(fake));
    port.context = &fake;
    port.write = fake_write;
    port.read = fake_read;
    port.delay_ms = fake_delay;
    fake.regs[0] = 0x00;

    CHECK(pca9685_init(&pwm, port, 0x40, 50) == 1);
    CHECK(fake.regs[0xFE] == 121);
    CHECK(fake.delays == 1);

    CHECK(pca9685_set_pwm(&pwm, 0, 0x123, 0x456) == 1);
    CHECK(fake.regs[0x06] == 0x23 && fake.regs[0x07] == 0x01);
    CHECK(fake.regs[0x08] == 0x56 && fake.regs[0x09] == 0x04);

    CHECK(pca9685_set_all(&pwm, 0, 0x100) == 1);
    CHECK(fake.regs[0xFA] == 0x00 && fake.regs[0xFB] == 0x00);
    CHECK(fake.regs[0xFC] == 0x00 && fake.regs[0xFD] == 0x01);

    CHECK(pca9685_set_frequency(&pwm, 60) == 1);
    CHECK(fake.regs[0xFE] == 101);
    CHECK(pca9685_set_pwm(&pwm, 16, 0, 0) == 0);
    CHECK(pca9685_set_frequency(&pwm, 0) == 0);

    if (failures) return 1;
    puts("pca9685: ok");
    return 0;
}
