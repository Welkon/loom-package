#include "sh1106.h"

static void command(sh1106_port_t port, uint8_t value)
{
    (void)port.write(port.context, 0x00U, &value, 1U);
}

void sh1106_init(sh1106_port_t port)
{
    static const uint8_t sequence[] = {
        0xAE, 0xA4, 0xD5, 0x50, 0xA8, 0x3F, 0xD3, 0x00,
        0x40, 0xAD, 0x8B, 0xD9, 0x22, 0xDB, 0x35, 0x32,
        0x81, 0xFF, 0xA6, 0xDA, 0x12, 0xAF,
    };
    uint8_t index;
    for (index = 0; index < sizeof(sequence); index++) {
        command(port, sequence[index]);
    }
}

void sh1106_clear(sh1106_port_t port)
{
    static const uint8_t zeros[16] = {0};
    uint8_t page;
    uint8_t chunk;
    for (page = 0; page < 8U; page++) {
        command(port, (uint8_t)(0xB0U | page));
        command(port, 0x02U);
        command(port, 0x10U);
        for (chunk = 0; chunk < 8U; chunk++) {
            (void)port.write(port.context, 0x40U, zeros, sizeof(zeros));
        }
    }
}

void sh1106_set_contrast(sh1106_port_t port, uint8_t value)
{
    command(port, 0x81U);
    command(port, value);
}

void sh1106_write_page(sh1106_port_t port, uint8_t page, uint8_t column,
                       const uint8_t *data, uint8_t width)
{
    uint8_t offset;
    if (data == 0 || width == 0U || page > 7U || column > 127U
        || (uint16_t)column + width > 128U) {
        return;
    }
    offset = (uint8_t)(column + 2U);
    command(port, (uint8_t)(0xB0U | page));
    command(port, (uint8_t)(offset & 0x0FU));
    command(port, (uint8_t)(0x10U | (offset >> 4)));
    (void)port.write(port.context, 0x40U, data, width);
}
