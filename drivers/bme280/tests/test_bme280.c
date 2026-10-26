#include <stdio.h>
#include <string.h>
#include "bme280.h"

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

static uint8_t fake_write(void *context, uint8_t address, uint8_t reg,
                          const uint8_t *data, uint16_t length)
{
    fake_t *fake = (fake_t *)context;
    (void)address;
    for (uint16_t i = 0; i < length; i++) fake->regs[reg + i] = data[i];
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

static void wr16(uint8_t *regs, uint8_t reg, uint16_t value)
{
    regs[reg] = (uint8_t)(value & 0xFF);
    regs[reg + 1] = (uint8_t)(value >> 8);
}

int main(void)
{
    fake_t fake;
    bme280_t sensor;
    bme280_port_t port;
    bme280_reading_t reading;

    memset(&fake, 0, sizeof(fake));
    port.context = &fake;
    port.write = fake_write;
    port.read = fake_read;
    port.delay_ms = fake_delay;
    fake.regs[0xD0] = 0x60;
    wr16(fake.regs, 0x88, 27504);
    wr16(fake.regs, 0x8A, 26435);
    wr16(fake.regs, 0x8C, (uint16_t)-1000);
    wr16(fake.regs, 0x8E, 36477);
    wr16(fake.regs, 0x90, (uint16_t)-10685);
    wr16(fake.regs, 0x92, 3024);
    wr16(fake.regs, 0x94, 2855);
    wr16(fake.regs, 0x96, 140);
    wr16(fake.regs, 0x98, (uint16_t)-7);
    wr16(fake.regs, 0x9A, 15500);
    wr16(fake.regs, 0x9C, (uint16_t)-14600);
    wr16(fake.regs, 0x9E, 6000);
    fake.regs[0xA1] = 75;
    wr16(fake.regs, 0xE1, 362);
    fake.regs[0xE3] = 0;
    fake.regs[0xE4] = 0x13;
    fake.regs[0xE5] = 0x28;
    fake.regs[0xE6] = 0x03;
    fake.regs[0xE7] = 30;
    CHECK(bme280_init(&sensor, port, 0x76) == 1);
    CHECK(fake.regs[0xF2] == 0x01 && fake.regs[0xF5] == 0xA0);
    CHECK(fake.delays == 1);

    /* raw pressure 415148, temperature 519888, humidity 30000 */
    fake.regs[0xF7] = (uint8_t)(415148 >> 12);
    fake.regs[0xF8] = (uint8_t)((415148 >> 4) & 0xFF);
    fake.regs[0xF9] = (uint8_t)((415148 & 0x0F) << 4);
    fake.regs[0xFA] = (uint8_t)(519888 >> 12);
    fake.regs[0xFB] = (uint8_t)((519888 >> 4) & 0xFF);
    fake.regs[0xFC] = (uint8_t)((519888 & 0x0F) << 4);
    fake.regs[0xFD] = (uint8_t)(30000 >> 8);
    fake.regs[0xFE] = (uint8_t)30000;
    CHECK(bme280_read(&sensor, &reading) == 1);
    CHECK(reading.temperature_milli_c == 25080);
    CHECK(reading.pressure_pa == 100653);
    CHECK(reading.humidity_milli_percent == 55353);
    CHECK(fake.delays == 2);

    fake.regs[0xD0] = 0x00;
    CHECK(bme280_init(&sensor, port, 0x76) == 0);

    if (failures) return 1;
    puts("bme280: ok");
    return 0;
}
