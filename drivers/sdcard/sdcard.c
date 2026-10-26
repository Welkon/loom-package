#include "sdcard.h"

static uint8_t send_command(sdcard_t *card, uint8_t command, uint32_t argument, uint8_t crc)
{
    uint8_t response;
    uint8_t i;
    card->port.select(card->port.context);
    (void)card->port.transfer(card->port.context, 0xFFU);
    (void)card->port.transfer(card->port.context, (uint8_t)(0x40U | command));
    (void)card->port.transfer(card->port.context, (uint8_t)(argument >> 24));
    (void)card->port.transfer(card->port.context, (uint8_t)(argument >> 16));
    (void)card->port.transfer(card->port.context, (uint8_t)(argument >> 8));
    (void)card->port.transfer(card->port.context, (uint8_t)argument);
    (void)card->port.transfer(card->port.context, crc);
    for (i = 0; i < 8U; i++) {
        response = card->port.transfer(card->port.context, 0xFFU);
        if (response != 0xFFU) {
            return response;
        }
    }
    return 0xFFU;
}

static void finish_command(sdcard_t *card)
{
    (void)card->port.transfer(card->port.context, 0xFFU);
    card->port.deselect(card->port.context);
}

uint8_t sdcard_init(sdcard_t *card, sdcard_port_t port)
{
    uint8_t response;
    uint16_t i;
    uint8_t r7[4];
    if (card == 0) {
        return 0;
    }
    card->port = port;
    card->initialized = 0;
    port.select(port.context);
    for (i = 0; i < 10U; i++) {
        (void)port.transfer(port.context, 0xFFU);
    }
    port.deselect(port.context);

    response = send_command(card, 0x00U, 0x00000000UL, 0x95U);
    finish_command(card);
    if (response != 0x01U) {
        return 0;
    }
    response = send_command(card, 0x08U, 0x000001AAUL, 0x87U);
    if (response != 0x01U) {
        finish_command(card);
        return 0;
    }
    for (i = 0; i < 4U; i++) {
        r7[i] = port.transfer(port.context, 0xFFU);
    }
    finish_command(card);
    if (r7[2] != 0x01U || r7[3] != 0xAAU) {
        return 0;
    }
    for (i = 0; i < 1000U; i++) {
        response = send_command(card, 0x37U, 0x00000000UL, 0x01U);
        finish_command(card);
        if (response > 0x01U) {
            return 0;
        }
        response = send_command(card, 0x29U, 0x40000000UL, 0x01U);
        finish_command(card);
        if (response == 0x00U) {
            break;
        }
        port.delay_ms(port.context, 1U);
    }
    if (i == 1000U) {
        return 0;
    }
    response = send_command(card, 0x3AU, 0x00000000UL, 0x01U);
    if (response != 0x00U) {
        finish_command(card);
        return 0;
    }
    for (i = 0; i < 4U; i++) {
        (void)port.transfer(port.context, 0xFFU);
    }
    finish_command(card);
    response = send_command(card, 0x10U, 512UL, 0x01U);
    finish_command(card);
    if (response != 0x00U) {
        return 0;
    }
    card->initialized = 1;
    return 1;
}

uint8_t sdcard_read_block(sdcard_t *card, uint32_t block, uint8_t data[512])
{
    uint8_t response;
    uint16_t i;
    uint16_t wait;
    if (card == 0 || card->initialized == 0U || data == 0) {
        return 0;
    }
    response = send_command(card, 0x11U, block << 9, 0x01U);
    if (response != 0x00U) {
        finish_command(card);
        return 0;
    }
    for (wait = 0; wait < 1000U; wait++) {
        response = card->port.transfer(card->port.context, 0xFFU);
        if (response == 0xFEU) {
            break;
        }
    }
    if (wait == 1000U) {
        finish_command(card);
        return 0;
    }
    for (i = 0; i < 512U; i++) {
        data[i] = card->port.transfer(card->port.context, 0xFFU);
    }
    (void)card->port.transfer(card->port.context, 0xFFU);
    (void)card->port.transfer(card->port.context, 0xFFU);
    finish_command(card);
    return 1;
}

uint8_t sdcard_write_block(sdcard_t *card, uint32_t block, const uint8_t data[512])
{
    uint8_t response;
    uint16_t i;
    uint16_t wait;
    if (card == 0 || card->initialized == 0U || data == 0) {
        return 0;
    }
    response = send_command(card, 0x18U, block << 9, 0x01U);
    if (response != 0x00U) {
        finish_command(card);
        return 0;
    }
    (void)card->port.transfer(card->port.context, 0xFEU);
    for (i = 0; i < 512U; i++) {
        (void)card->port.transfer(card->port.context, data[i]);
    }
    (void)card->port.transfer(card->port.context, 0xFFU);
    (void)card->port.transfer(card->port.context, 0xFFU);
    response = card->port.transfer(card->port.context, 0xFFU);
    if ((response & 0x1FU) != 0x05U) {
        finish_command(card);
        return 0;
    }
    for (wait = 0; wait < 1000U; wait++) {
        if (card->port.transfer(card->port.context, 0xFFU) == 0xFFU) {
            break;
        }
    }
    finish_command(card);
    return wait < 1000U ? 1U : 0U;
}
