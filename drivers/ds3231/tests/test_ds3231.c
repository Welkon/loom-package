#include <stdio.h>
#include <string.h>
#include "ds3231.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

typedef struct {
    uint8_t regs[0x13];
    uint8_t pointer;
} fake_t;

static uint8_t fake_write(void *context, uint8_t address, const uint8_t *data, uint16_t length)
{
    fake_t *fake = (fake_t *)context;
    (void)address;
    if (length == 0) return 0;
    fake->pointer = data[0];
    for (uint16_t i = 1; i < length; i++) {
        fake->regs[fake->pointer++] = data[i];
    }
    return 1;
}

static uint8_t fake_read(void *context, uint8_t address, uint8_t *data, uint16_t length)
{
    fake_t *fake = (fake_t *)context;
    (void)address;
    for (uint16_t i = 0; i < length; i++) {
        data[i] = fake->regs[fake->pointer++];
    }
    return 1;
}

int main(void)
{
    fake_t fake;
    ds3231_port_t port;
    ds3231_time_t time;
    int32_t milli;
    uint8_t lost;

    memset(&fake, 0, sizeof(fake));
    port.context = &fake;
    port.write = fake_write;
    port.read = fake_read;

    fake.regs[0] = 0x56; fake.regs[1] = 0x34; fake.regs[2] = 0x12;
    fake.regs[3] = 0x02; fake.regs[4] = 0x14; fake.regs[5] = 0x07;
    fake.regs[6] = 0x26;
    CHECK(ds3231_read_time(port, &time) == 1);
    CHECK(time.second == 56 && time.minute == 34 && time.hour == 12);
    CHECK(time.date == 14 && time.month == 7 && time.year == 2026);

    time.second = 5; time.minute = 4; time.hour = 3; time.day = 6;
    time.date = 2; time.month = 1; time.year = 2027;
    fake.regs[0x0F] = 0x80;
    CHECK(ds3231_write_time(port, &time) == 1);
    CHECK(fake.regs[0] == 0x05 && fake.regs[1] == 0x04 && fake.regs[2] == 0x03);
    CHECK(fake.regs[6] == 0x27);
    CHECK((fake.regs[0x0F] & 0x80) == 0);

    fake.regs[0x11] = 0x19; fake.regs[0x12] = 0x40;
    CHECK(ds3231_read_temperature(port, &milli) == 1 && milli == 25250);
    fake.regs[0x11] = 0xF5; fake.regs[0x12] = 0x80;
    CHECK(ds3231_read_temperature(port, &milli) == 1 && milli == -10500);

    fake.regs[5] = 0x85; fake.regs[6] = 0x05;
    CHECK(ds3231_read_time(port, &time) == 1 && time.year == 2105);
    time.year = 2105; time.month = 5; time.date = 1; time.hour = 0;
    time.minute = 0; time.second = 0; time.day = 1;
    CHECK(ds3231_write_time(port, &time) == 1);
    CHECK((fake.regs[5] & 0x80) != 0 && fake.regs[6] == 0x05);

    fake.regs[0x0F] = 0x80;
    CHECK(ds3231_lost_power(port, &lost) == 1 && lost == 1);
    fake.regs[0x0F] = 0x00;
    CHECK(ds3231_lost_power(port, &lost) == 1 && lost == 0);

    if (failures) return 1;
    puts("ds3231: ok");
    return 0;
}
