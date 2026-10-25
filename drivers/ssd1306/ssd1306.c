#include "ssd1306.h"

static uint8_t command(ssd1306_port_t port, uint8_t value)
{
    return port.write(port.context, 0x00U, &value, 1U);
}

void ssd1306_init(ssd1306_port_t port)
{
    static const uint8_t sequence[] = {
        0xAE, 0xD5, 0x80, 0xA8, 0x3F, 0xD3, 0x00, 0x40,
        0x8D, 0x14, 0x20, 0x00, 0xA1, 0xC8, 0xDA, 0x12,
        0x81, 0xCF, 0xD9, 0xF1, 0xDB, 0x40, 0xA4, 0xA6,
        0x2E, 0xAF,
    };
    uint8_t index;
    for (index = 0; index < sizeof(sequence); index++) {
        (void)command(port, sequence[index]);
    }
}

void ssd1306_clear(ssd1306_port_t port)
{
    static const uint8_t zeros[16] = {0};
    uint8_t page;
    uint8_t chunk;
    for (page = 0; page < 8U; page++) {
        (void)command(port, (uint8_t)(0xB0U | page));
        (void)command(port, 0x00U);
        (void)command(port, 0x10U);
        for (chunk = 0; chunk < 8U; chunk++) {
            (void)port.write(port.context, 0x40U, zeros, sizeof(zeros));
        }
    }
}

void ssd1306_set_contrast(ssd1306_port_t port, uint8_t value)
{
    (void)command(port, 0x81U);
    (void)command(port, value);
}

void ssd1306_write_page(ssd1306_port_t port, uint8_t page, uint8_t column,
                        const uint8_t *data, uint8_t width)
{
    if (data == 0 || width == 0U || page > 7U || column > 127U
        || (uint16_t)column + width > 128U) {
        return;
    }
    (void)command(port, (uint8_t)(0xB0U | page));
    (void)command(port, (uint8_t)(column & 0x0FU));
    (void)command(port, (uint8_t)(0x10U | (column >> 4)));
    (void)port.write(port.context, 0x40U, data, width);
}
