#include <stdio.h>
#include <string.h>
#include "mcp2515.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

typedef struct {
    uint8_t regs[128];
    uint8_t instruction;
    uint8_t address;
    uint8_t mask;
    uint8_t phase;
    uint16_t resets;
    uint16_t sent;
    uint16_t delays;
} fake_t;

static void fake_select(void *context)
{
    fake_t *fake = (fake_t *)context;
    fake->phase = 0;
}

static uint8_t fake_transfer(void *context, uint8_t value)
{
    fake_t *fake = (fake_t *)context;
    if (fake->phase == 0) {
        fake->instruction = value;
        fake->phase = 1;
        if (value == 0xC0) {
            memset(fake->regs, 0, sizeof(fake->regs));
            fake->resets++;
        } else if (value == 0x81) {
            fake->sent++;
        }
        return 0;
    }
    if (fake->instruction == 0x02) {
        if (fake->phase == 1) {
            fake->address = value;
            fake->phase = 2;
        } else {
            fake->regs[fake->address++] = value;
        }
        return 0;
    }
    if (fake->instruction == 0x03) {
        if (fake->phase == 1) {
            fake->address = value;
            fake->phase = 2;
            return 0;
        }
        return fake->regs[fake->address++];
    }
    if (fake->instruction == 0x05) {
        if (fake->phase == 1) {
            fake->address = value;
            fake->phase = 2;
        } else if (fake->phase == 2) {
            fake->mask = value;
            fake->phase = 3;
        } else {
            fake->regs[fake->address] =
                (uint8_t)((fake->regs[fake->address] & ~fake->mask) | (value & fake->mask));
        }
        return 0;
    }
    return 0;
}

static void fake_deselect(void *context) { (void)context; }
static void fake_delay(void *context, uint16_t ms)
{
    fake_t *fake = (fake_t *)context;
    (void)ms;
    fake->delays++;
}

int main(void)
{
    fake_t fake;
    mcp2515_t can;
    mcp2515_port_t port;
    can_frame_t frame;

    memset(&fake, 0, sizeof(fake));
    port.context = &fake;
    port.select = fake_select;
    port.transfer = fake_transfer;
    port.deselect = fake_deselect;
    port.delay_ms = fake_delay;

    CHECK(mcp2515_init(&can, port, 16, 500) == 1);
    CHECK(fake.resets == 1);
    CHECK(fake.regs[0x2A] == 0x00 && fake.regs[0x29] == 0xF0 && fake.regs[0x28] == 0x86);
    CHECK(fake.regs[0x2B] == 0x03);
    CHECK((fake.regs[0x0F] & 0xE0) == 0);

    memset(&frame, 0, sizeof(frame));
    frame.id = 0x123; frame.dlc = 2; frame.data[0] = 0x11; frame.data[1] = 0x22;
    CHECK(mcp2515_send(&can, &frame) == 1);
    CHECK(fake.sent == 1);
    CHECK(fake.regs[0x31] == 0x24 && fake.regs[0x32] == 0x60);
    CHECK(fake.regs[0x35] == 0x02 && fake.regs[0x36] == 0x11 && fake.regs[0x37] == 0x22);

    fake.regs[0x2C] = 0x01;
    fake.regs[0x61] = 0x24; fake.regs[0x62] = 0x60;
    fake.regs[0x63] = 0x00; fake.regs[0x64] = 0x00;
    fake.regs[0x65] = 0x01; fake.regs[0x66] = 0x7F;
    CHECK(mcp2515_receive(&can, &frame) == 1);
    CHECK(frame.id == 0x123 && frame.extended == 0 && frame.dlc == 1);
    CHECK(frame.data[0] == 0x7F);
    CHECK((fake.regs[0x2C] & 0x03) == 0);

    fake.regs[0x2C] = 0x02;
    fake.regs[0x71] = 0x00; fake.regs[0x72] = 0x0C;
    fake.regs[0x73] = 0x12; fake.regs[0x74] = 0x34;
    fake.regs[0x75] = 0x00;
    fake.regs[0x76] = 0xAA;
    CHECK(mcp2515_receive(&can, &frame) == 1);
    CHECK(frame.extended == 1 && frame.id == 0x1234 && frame.dlc == 0);

    fake.regs[0x2C] = 0;
    CHECK(mcp2515_receive(&can, &frame) == 0);
    CHECK(mcp2515_init(&can, port, 12, 500) == 0);
    CHECK(mcp2515_init(&can, port, 16, 123) == 0);

    if (failures) return 1;
    puts("mcp2515: ok");
    return 0;
}
