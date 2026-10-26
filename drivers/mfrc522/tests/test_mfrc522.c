#include <stdio.h>
#include <string.h>
#include "mfrc522.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } \
} while (0)

typedef struct {
    uint8_t regs[64];
    uint8_t fifo[32];
    uint8_t fifo_length;
    uint8_t response[2][8];
    uint8_t response_length[2];
    uint8_t response_index;
    uint8_t active;
    uint8_t read_phase;
    uint8_t phase;
    uint8_t address;
    uint16_t delays;
} fake_t;

static void fake_select(void *context) { fake_t *f = (fake_t *)context; f->phase = 0; }
static uint8_t fake_transfer(void *context, uint8_t value)
{
    fake_t *f = (fake_t *)context;
    if (f->phase == 0) {
        f->address = value;
        f->phase = 1;
        return 0;
    }
    f->phase = 0;
    if ((f->address & 0x80U) != 0U) {
        uint8_t reg = (uint8_t)((f->address >> 1) & 0x3FU);
        if (reg == 0x04U) return 0x30U;
        if (reg == 0x06U) return 0;
        if (reg == 0x0AU) { f->active = f->response_index; return f->response_length[f->active]; }
        if (reg == 0x09U) {
            uint8_t value_out = f->response[f->active][f->read_phase++];
            if (f->read_phase >= f->response_length[f->active]) {
                f->read_phase = 0;
                f->response_index++;
            }
            return value_out;
        }
        return f->regs[reg];
    }
    uint8_t reg = (uint8_t)((f->address >> 1) & 0x3FU);
    if (reg == 0x09U) {
        f->fifo[f->fifo_length++] = value;
        if (value == 0x0CU) {
            f->read_phase = 0;
        }
    } else if (reg == 0x01U && value == 0x0CU) {
        f->read_phase = 0;
    } else if (reg == 0x01U && value == 0x0FU) {
        f->response_index = 0;
    } else {
        f->regs[reg] = value;
    }
    return 0;
}
static void fake_deselect(void *context) { (void)context; }
static void fake_reset(void *context, uint8_t level) { (void)context; (void)level; }
static void fake_delay(void *context, uint16_t ms) { fake_t *f = (fake_t *)context; (void)ms; f->delays++; }

int main(void)
{
    fake_t fake;
    mfrc522_t reader;
    mfrc522_port_t port;
    uint16_t atqa;
    uint8_t full[5];
    uint8_t uid[4];

    memset(&fake, 0, sizeof(fake));
    fake.regs[0x37] = 0x92;
    fake.response[0][0] = 0x04; fake.response[0][1] = 0x00; fake.response_length[0] = 2;
    fake.response[1][0] = 0xDE; fake.response[1][1] = 0xAD;
    fake.response[1][2] = 0xBE; fake.response[1][3] = 0xEF;
    fake.response[1][4] = (uint8_t)(0xDE ^ 0xAD ^ 0xBE ^ 0xEF);
    fake.response_length[1] = 5;
    port.context = &fake;
    port.select = fake_select;
    port.transfer = fake_transfer;
    port.deselect = fake_deselect;
    port.reset = fake_reset;
    port.delay_ms = fake_delay;
    CHECK(mfrc522_init(&reader, port) == 1);
    CHECK(fake.regs[0x15] == 0x40 && fake.regs[0x11] == 0x3D);
    CHECK(fake.delays >= 3);

    CHECK(mfrc522_request(&reader, &atqa) == 1 && atqa == 0x0400);
    CHECK(mfrc522_anticoll(&reader, full) == 1);
    CHECK(full[0] == 0xDE && full[3] == 0xEF);

    fake.response_index = 0;
    CHECK(mfrc522_read_uid(&reader, uid) == 1);
    CHECK(uid[0] == 0xDE && uid[3] == 0xEF);

    fake.response_length[0] = 0;
    fake.response_index = 0;
    CHECK(mfrc522_request(&reader, &atqa) == 0);

    if (failures) return 1;
    puts("mfrc522: ok");
    return 0;
}
