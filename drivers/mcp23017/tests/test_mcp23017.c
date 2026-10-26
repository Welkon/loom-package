#include <stdio.h>
#include <string.h>
#include "mcp23017.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

typedef struct {
    uint8_t regs[32];
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

int main(void)
{
    fake_t fake;
    mcp23017_t expander;
    mcp23017_port_t port;
    uint8_t value;

    memset(&fake, 0, sizeof(fake));
    port.context = &fake;
    port.write = fake_write;
    port.read = fake_read;
    CHECK(mcp23017_init(&expander, port, 0x20) == 1);
    CHECK(fake.regs[0x00] == 0xFF && fake.regs[0x01] == 0xFF);
    CHECK(fake.regs[0x0A] == 0x00);

    CHECK(mcp23017_pin_mode(&expander, 0, 1) == 1);
    CHECK((fake.regs[0x00] & 0x01) == 0);
    CHECK(mcp23017_write_pin(&expander, 0, 1) == 1);
    CHECK((fake.regs[0x14] & 0x01) == 1);
    CHECK(mcp23017_write_pin(&expander, 9, 1) == 1);
    CHECK((fake.regs[0x15] & 0x02) == 2);

    fake.regs[0x12] = 0x80;
    CHECK(mcp23017_read_pin(&expander, 7, &value) == 1 && value == 1);
    fake.regs[0x13] = 0x01;
    CHECK(mcp23017_read_port(&expander, 1, &value) == 1 && value == 0x01);

    CHECK(mcp23017_set_pullup(&expander, 4, 1) == 1);
    CHECK((fake.regs[0x0C] & 0x10) == 0x10);
    CHECK(mcp23017_read_pin(&expander, 16, &value) == 0);

    if (failures) return 1;
    puts("mcp23017: ok");
    return 0;
}
