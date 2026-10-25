#include "max7219.h"

static void frame(max7219_t *driver, uint8_t reg, uint8_t value)
{
    (void)driver->port.transfer(driver->port.context, reg);
    (void)driver->port.transfer(driver->port.context, value);
}

static void write_chain(max7219_t *driver, uint8_t device, uint8_t reg, uint8_t value)
{
    uint8_t i;
    if (driver == 0 || device >= driver->devices) {
        return;
    }
    driver->port.select(driver->port.context);
    for (i = driver->devices; i > 0U; i--) {
        if ((uint8_t)(i - 1U) == device) {
            frame(driver, reg, value);
        } else {
            frame(driver, 0x00U, 0x00U);
        }
    }
    driver->port.deselect(driver->port.context);
}

void max7219_init(max7219_t *driver, max7219_port_t port, uint8_t devices)
{
    uint8_t device;
    uint8_t digit;
    if (driver == 0) {
        return;
    }
    driver->port = port;
    driver->devices = devices > 8U ? 8U : devices;
    if (driver->devices == 0U) {
        driver->devices = 1U;
    }
    for (device = 0; device < driver->devices; device++) {
        max7219_write(driver, device, 0x0F, 0x00);
        max7219_write(driver, device, 0x0B, 0x07);
        max7219_write(driver, device, 0x09, 0x00);
        max7219_write(driver, device, 0x0A, 0x08);
        for (digit = 1; digit <= 8U; digit++) {
            max7219_write(driver, device, digit, 0x00);
        }
        max7219_write(driver, device, 0x0C, 0x01);
    }
}

void max7219_write(max7219_t *driver, uint8_t device, uint8_t reg, uint8_t value)
{
    write_chain(driver, device, reg, value);
}

void max7219_set_digit(max7219_t *driver, uint8_t device, uint8_t digit, uint8_t value)
{
    if (digit < 8U) {
        write_chain(driver, device, (uint8_t)(digit + 1U), (uint8_t)(value & 0x0FU));
    }
}

void max7219_set_row(max7219_t *driver, uint8_t device, uint8_t row, uint8_t value)
{
    if (row < 8U) {
        write_chain(driver, device, (uint8_t)(row + 1U), value);
    }
}

void max7219_set_intensity(max7219_t *driver, uint8_t device, uint8_t value)
{
    write_chain(driver, device, 0x0AU, (uint8_t)(value & 0x0FU));
}

void max7219_shutdown(max7219_t *driver, uint8_t device, uint8_t shutdown)
{
    write_chain(driver, device, 0x0CU, shutdown ? 0x00U : 0x01U);
}

void max7219_clear(max7219_t *driver, uint8_t device)
{
    uint8_t digit;
    for (digit = 1; digit <= 8U; digit++) {
        write_chain(driver, device, digit, 0x00U);
    }
}
