#include <stdio.h>
#include <string.h>
#include "nrf24l01.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } \
} while (0)

typedef struct {
    uint8_t regs[64];
    uint8_t command;
    uint8_t address;
    uint8_t phase;
    uint8_t payload[32];
    uint8_t payload_length;
    uint8_t ce;
    uint8_t status;
    uint16_t delays;
} fake_t;

static void fake_select(void *context) { fake_t *f = (fake_t *)context; f->phase = 0; }
static uint8_t fake_transfer(void *context, uint8_t value)
{
    fake_t *f = (fake_t *)context;
    if (f->phase == 0) { f->command = value; f->phase = 1; return 0; }
    if ((f->command & 0xE0) == 0x00) {
        if (f->phase == 1) { f->address = f->command & 0x1F; f->phase = 2; }
        return f->regs[f->address++];
    }
    if ((f->command & 0xE0) == 0x20) {
        if (f->phase == 1) { f->address = f->command & 0x1F; f->phase = 2; }
        f->regs[f->address++] = value;
        return 0;
    }
    if (f->command == 0xA0) { f->payload[f->payload_length++] = value; return 0; }
    if (f->command == 0x61) { return f->payload[f->payload_length++]; }
    return 0;
}
static void fake_deselect(void *context) { (void)context; }
static void fake_ce(void *context, uint8_t level) { ((fake_t *)context)->ce = level; }
static void fake_delay(void *context, uint16_t ms) { fake_t *f = (fake_t *)context; (void)ms; f->delays++; }

int main(void)
{
    fake_t fake;
    nrf24l01_t radio;
    nrf24l01_port_t port;
    const uint8_t address[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
    uint8_t payload[4] = {1, 2, 3, 4};
    uint8_t received[4];
    uint8_t length = 0;

    memset(&fake, 0, sizeof(fake));
    port.context = &fake;
    port.select = fake_select;
    port.transfer = fake_transfer;
    port.deselect = fake_deselect;
    port.ce = fake_ce;
    port.delay_ms = fake_delay;
    CHECK(nrf24l01_init(&radio, port, 76, address, 4) == 1);
    CHECK(fake.regs[0x00] == 0x0E && fake.regs[0x05] == 76 && fake.regs[0x11] == 4);
    CHECK(fake.delays == 1);

    fake.regs[0x07] = 0x20; /* TX_DS */
    CHECK(nrf24l01_send(&radio, payload, sizeof(payload)) == 1);
    CHECK(fake.ce == 0 && fake.payload_length == 4);
    CHECK(fake.payload[0] == 1 && fake.payload[3] == 4);

    fake.regs[0x07] = 0x40; /* RX_DR */
    fake.payload_length = 0;
    memcpy(fake.payload, (uint8_t[]){9, 8, 7, 6}, 4);
    CHECK(nrf24l01_receive(&radio, received, sizeof(received), &length) == 1);
    CHECK(length == 4 && received[0] == 9 && received[3] == 6);

    fake.regs[0x07] = 0;
    CHECK(nrf24l01_receive(&radio, received, sizeof(received), &length) == 0);

    if (failures) return 1;
    puts("nrf24l01: ok");
    return 0;
}
