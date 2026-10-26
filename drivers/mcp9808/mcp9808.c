#include "mcp9808.h"

#define REG_CONFIG 0x01U
#define REG_TEMPERATURE 0x05U

uint8_t mcp9808_init(mcp9808_t *device, mcp9808_port_t port, uint8_t i2c_address)
{
    uint8_t config[2] = {0x00U, 0x03U}; /* continuous, +-0.0625 C */
    if (device == 0) {
        return 0;
    }
    device->port = port;
    device->i2c_address = i2c_address;
    return port.write(port.context, i2c_address, REG_CONFIG,
                      config, sizeof(config)) ? 1U : 0U;
}

uint8_t mcp9808_read(mcp9808_t *device, int32_t *milli_celsius)
{
    uint8_t data[2];
    int32_t raw;
    if (device == 0 || milli_celsius == 0
        || !device->port.read(device->port.context, device->i2c_address,
                              REG_TEMPERATURE, data, sizeof(data))) {
        return 0;
    }
    raw = (int32_t)(((uint16_t)data[0] << 8) | data[1]) & 0x1FFFU;
    if ((raw & 0x1000U) != 0U) {
        raw -= 0x2000;
    }
    *milli_celsius = raw * 125 / 2;
    return 1;
}
