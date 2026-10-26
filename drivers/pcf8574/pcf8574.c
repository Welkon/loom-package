#include "pcf8574.h"

uint8_t pcf8574_init(pcf8574_t *device, pcf8574_port_t port, uint8_t i2c_address)
{
    if (device == 0) {
        return 0;
    }
    device->port = port;
    device->i2c_address = i2c_address;
    device->output = 0xFFU;
    return port.write(port.context, i2c_address, &device->output, 1U) ? 1U : 0U;
}

uint8_t pcf8574_write_port(pcf8574_t *device, uint8_t value)
{
    if (device == 0) {
        return 0;
    }
    if (!device->port.write(device->port.context, device->i2c_address, &value, 1U)) {
        return 0;
    }
    device->output = value;
    return 1;
}

uint8_t pcf8574_read_port(pcf8574_t *device, uint8_t *value)
{
    if (device == 0 || value == 0) {
        return 0;
    }
    return device->port.read(device->port.context, device->i2c_address, value, 1U) ? 1U : 0U;
}

uint8_t pcf8574_write_pin(pcf8574_t *device, uint8_t pin, uint8_t value)
{
    uint8_t next;
    if (device == 0 || pin > 7U) {
        return 0;
    }
    next = value ? (uint8_t)(device->output | (1U << pin))
                 : (uint8_t)(device->output & (uint8_t)~(1U << pin));
    return pcf8574_write_port(device, next);
}

uint8_t pcf8574_read_pin(pcf8574_t *device, uint8_t pin, uint8_t *value)
{
    uint8_t port_value;
    if (device == 0 || value == 0 || pin > 7U
        || !pcf8574_read_port(device, &port_value)) {
        return 0;
    }
    *value = (uint8_t)((port_value >> pin) & 1U);
    return 1;
}
