#include "hd44780.h"

static void pulse(hd44780_t *lcd)
{
    lcd->port.set_enable(lcd->port.context, 1U);
    lcd->port.set_enable(lcd->port.context, 0U);
}

static void nibble(hd44780_t *lcd, uint8_t value)
{
    lcd->port.write_nibble(lcd->port.context, (uint8_t)(value & 0x0FU));
    pulse(lcd);
}

static void send(hd44780_t *lcd, uint8_t value, uint8_t rs)
{
    lcd->port.set_rs(lcd->port.context, rs ? 1U : 0U);
    nibble(lcd, (uint8_t)(value >> 4));
    nibble(lcd, value);
}

static void command(hd44780_t *lcd, uint8_t value, uint8_t delay)
{
    send(lcd, value, 0U);
    lcd->port.delay_ms(lcd->port.context, delay);
}

uint8_t hd44780_init(hd44780_t *lcd, hd44780_port_t port, uint8_t columns, uint8_t rows)
{
    if (lcd == 0 || columns == 0U || columns > 20U || rows == 0U || rows > 2U) {
        return 0;
    }
    lcd->port = port;
    lcd->columns = columns;
    lcd->rows = rows;
    port.set_rs(port.context, 0U);
    port.delay_ms(port.context, 50U);
    nibble(lcd, 0x03U);
    port.delay_ms(port.context, 5U);
    nibble(lcd, 0x03U);
    port.delay_ms(port.context, 1U);
    nibble(lcd, 0x03U);
    nibble(lcd, 0x02U);
    command(lcd, 0x28U, 1U); /* 4-bit, 2 lines, 5x8 */
    command(lcd, 0x08U, 1U); /* display off */
    command(lcd, 0x01U, 2U); /* clear */
    command(lcd, 0x06U, 1U); /* increment, no shift */
    command(lcd, 0x0CU, 1U); /* display on, cursor off */
    return 1;
}

void hd44780_clear(hd44780_t *lcd)
{
    if (lcd != 0) {
        command(lcd, 0x01U, 2U);
    }
}

void hd44780_set_cursor(hd44780_t *lcd, uint8_t column, uint8_t row)
{
    uint8_t address;
    if (lcd == 0 || column >= lcd->columns || row >= lcd->rows) {
        return;
    }
    address = (uint8_t)((row == 0U ? 0x00U : 0x40U) + column);
    command(lcd, (uint8_t)(0x80U | address), 1U);
}

void hd44780_write_char(hd44780_t *lcd, char value)
{
    if (lcd != 0) {
        send(lcd, (uint8_t)value, 1U);
        lcd->port.delay_ms(lcd->port.context, 1U);
    }
}

void hd44780_write_string(hd44780_t *lcd, const char *text)
{
    if (lcd == 0 || text == 0) {
        return;
    }
    while (*text != '\0') {
        hd44780_write_char(lcd, *text++);
    }
}
