#include "mcp4725.h"

uint8_t mcp4725_init(mcp4725_t *device, mcp4725_port_t port, uint8_t i2c_address)
{
    if (device == 0) {
        return 0;
    }
    device->port = port;
    device->i2c_address = i2c_address;
    return 1;
}

uint8_t mcp4725_write_value(mcp4725_t *device, uint16_t value)
{
    uint8_t data[2];
    if (device == 0 || value > 4095U) {
        return 0;
    }
    /* DAC mode: high nibble, then low byte; power-down bits are zero. */
    data[0] = (uint8_t)((value >> 8) & 0x0FU);
    data[1] = (uint8_t)(value & 0xFFU);
    return device->port.write(device->port.context, device->i2c_address,
                              data, sizeof(data)) ? 1U : 0U;
}
