#include <stdio.h>
#include <string.h>
#include "ads1115.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

typedef struct {
    uint8_t config[2];
    uint8_t conversion[2];
    uint16_t writes;
    uint16_t delays;
} fake_t;

static uint8_t fake_write(void *context, uint8_t address, uint8_t reg,
                          const uint8_t *data, uint16_t length)
{
    fake_t *fake = (fake_t *)context;
    (void)address;
    if (reg == 0x01U && length == 2) {
        fake->config[0] = data[0];
        fake->config[1] = data[1];
        fake->writes++;
        return 1;
    }
    return 0;
}

static uint8_t fake_read(void *context, uint8_t address, uint8_t reg,
                         uint8_t *data, uint16_t length)
{
    fake_t *fake = (fake_t *)context;
    (void)address;
    if (reg == 0x00U && length == 2) {
        data[0] = fake->conversion[0];
        data[1] = fake->conversion[1];
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
    ads1115_t adc;
    ads1115_port_t port;
    int16_t raw;
    int32_t microvolts;

    memset(&fake, 0, sizeof(fake));
    port.context = &fake;
    port.write = fake_write;
    port.read = fake_read;
    port.delay_ms = fake_delay;
    CHECK(ads1115_init(&adc, port, 0x48) == 1);

    fake.conversion[0] = 0x7F; fake.conversion[1] = 0xFF;
    CHECK(ads1115_read_single(&adc, 0, ADS1115_RANGE_4P096V, &raw, &microvolts) == 1);
    CHECK(raw == 32767 && microvolts == 4095875);
    CHECK(fake.config[0] == 0x83 && fake.config[1] == 0x83);
    CHECK(fake.writes == 1 && fake.delays == 1);

    fake.conversion[0] = 0x80; fake.conversion[1] = 0x00;
    CHECK(ads1115_read_single(&adc, 3, ADS1115_RANGE_2P048V, &raw, &microvolts) == 1);
    CHECK(raw == -32768 && microvolts == -2048000);
    CHECK((fake.config[0] & 0x70) == 0x30);

    CHECK(ads1115_read_single(&adc, 4, ADS1115_RANGE_4P096V, &raw, &microvolts) == 0);
    CHECK(ads1115_read_single(&adc, 0, (ads1115_range_t)6, &raw, &microvolts) == 0);

    if (failures) return 1;
    puts("ads1115: ok");
    return 0;
}
