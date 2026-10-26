#include <stdio.h>
#include <string.h>
#include "sdcard.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } \
} while (0)

typedef struct {
    uint8_t command;
    uint8_t argument[4];
    uint8_t crc;
    uint8_t index;
    uint8_t response;
    uint8_t extra[4];
    uint8_t extra_length;
    uint8_t extra_index;
    uint8_t data[512];
    uint16_t data_index;
    uint16_t write_count;
    uint8_t write_phase;
    uint8_t write_crc;
    uint8_t writes;
} fake_t;

static void fake_select(void *context) { fake_t *f = (fake_t *)context; f->index = 0; f->response = 0xFF; f->extra_index = 0; f->write_phase = 0; f->write_count = 0; f->write_crc = 0; }
static uint8_t fake_transfer(void *context, uint8_t value)
{
    fake_t *f = (fake_t *)context;
    if (f->index == 0 && (value & 0xC0U) == 0x40U) {
        f->command = value & 0x3FU;
        f->index = 1;
        return 0xFF;
    }
    if (f->index >= 1 && f->index <= 4) {
        f->argument[f->index - 1] = value;
        f->index++;
        return 0xFF;
    }
    if (f->index == 5) {
        f->crc = value;
        f->index = 6;
        f->response = 0xFF;
        f->extra_length = 0;
        if (f->command == 0x00) f->response = 0x01;
        if (f->command == 0x08) { f->response = 0x01; f->extra[0] = 0; f->extra[1] = 0; f->extra[2] = 1; f->extra[3] = 0xAA; f->extra_length = 4; }
        if (f->command == 0x37) f->response = 0x01;
        if (f->command == 0x29) f->response = 0x00;
        if (f->command == 0x3A) { f->response = 0x00; f->extra_length = 4; }
        if (f->command == 0x10) f->response = 0x00;
        if (f->command == 0x11) { f->response = 0x00; }
        if (f->command == 0x18) { f->response = 0x00; }
        return 0xFF;
    }
    if (f->index == 6) {
        f->index = 7;
        return f->response;
    }
    if (f->extra_index < f->extra_length) {
        return f->extra[f->extra_index++];
    }
    if (f->command == 0x11) {
        if (f->data_index == 0) { f->data_index = 1; return 0xFE; }
        if (f->data_index <= 512) return f->data[f->data_index++ - 1];
        f->data_index++;
        return 0xFF;
    }
    if (f->command == 0x18) {
        if (f->write_phase == 0 && value == 0xFE) { f->write_phase = 1; return 0xFF; }
        if (f->write_phase == 1) {
            f->data[f->write_count++] = value;
            if (f->write_count == 512) f->write_phase = 2;
            return 0xFF;
        }
        if (f->write_phase == 2) {
            f->write_crc++;
            if (f->write_crc == 2) f->write_phase = 3;
            return 0xFF;
        }
        if (f->write_phase == 3) {
            f->write_phase = 4;
            f->writes = 1;
            return 0x05;
        }
        return 0xFF;
    }
    return 0xFF;
}
static void fake_deselect(void *context) { (void)context; }
static void fake_delay(void *context, uint16_t ms) { (void)context; (void)ms; }

int main(void)
{
    fake_t fake;
    sdcard_t card;
    sdcard_port_t port;
    uint8_t block[512];
    uint8_t written[512];

    memset(&fake, 0, sizeof(fake));
    port.context = &fake;
    port.select = fake_select;
    port.transfer = fake_transfer;
    port.deselect = fake_deselect;
    port.delay_ms = fake_delay;
    CHECK(sdcard_init(&card, port) == 1);
    CHECK(card.initialized == 1);

    for (uint16_t i = 0; i < 512; i++) fake.data[i] = (uint8_t)(i & 0xFFU);
    fake.data_index = 0;
    CHECK(sdcard_read_block(&card, 0, block) == 1);
    CHECK(block[0] == 0 && block[255] == 255);

    for (uint16_t i = 0; i < 512; i++) written[i] = (uint8_t)(255U - (i & 0xFFU));
    fake.data_index = 0;
    CHECK(sdcard_write_block(&card, 2, written) == 1);
    CHECK(fake.writes == 1);
    CHECK(fake.data[0] == 255 && fake.data[1] == 254);

    if (failures) return 1;
    puts("sdcard: ok");
    return 0;
}
