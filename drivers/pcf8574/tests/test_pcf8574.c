#include <stdio.h>
#include <string.h>
#include "pcf8574.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

typedef struct {
    uint8_t value;
    uint16_t writes;
} fake_t;

static uint8_t fake_write(void *context, uint8_t address, const uint8_t *data, uint16_t length)
{
    fake_t *fake = (fake_t *)context;
    (void)address;
    if (length != 1) return 0;
    fake->value = data[0];
    fake->writes++;
    return 1;
}

static uint8_t fake_read(void *context, uint8_t address, uint8_t *data, uint16_t length)
{
    fake_t *fake = (fake_t *)context;
    (void)address;
    if (length != 1) return 0;
    data[0] = fake->value;
    return 1;
}

int main(void)
{
    fake_t fake;
    pcf8574_t expander;
    pcf8574_port_t port;
    uint8_t value;

    memset(&fake, 0, sizeof(fake));
    port.context = &fake;
    port.write = fake_write;
    port.read = fake_read;
    CHECK(pcf8574_init(&expander, port, 0x20) == 1);
    CHECK(fake.value == 0xFF && fake.writes == 1);

    CHECK(pcf8574_write_port(&expander, 0x5A) == 1);
    CHECK(fake.value == 0x5A && expander.output == 0x5A);
    CHECK(pcf8574_write_pin(&expander, 0, 0) == 1);
    CHECK(fake.value == 0x5A);
    CHECK(pcf8574_write_pin(&expander, 1, 0) == 1);
    CHECK(fake.value == 0x58);

    fake.value = 0xA5;
    CHECK(pcf8574_read_pin(&expander, 0, &value) == 1 && value == 1);
    CHECK(pcf8574_read_pin(&expander, 1, &value) == 1 && value == 0);
    CHECK(pcf8574_read_port(&expander, &value) == 1 && value == 0xA5);
    CHECK(pcf8574_write_pin(&expander, 8, 1) == 0);

    if (failures) return 1;
    puts("pcf8574: ok");
    return 0;
}
