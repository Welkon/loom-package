#include "ads1115.h"

#define REG_CONVERSION 0x00U
#define REG_CONFIG 0x01U

static const int32_t full_scale_uv[] = {
    6144000, 4096000, 2048000, 1024000, 512000, 256000,
};

uint8_t ads1115_init(ads1115_t *device, ads1115_port_t port, uint8_t i2c_address)
{
    if (device == 0) {
        return 0;
    }
    device->port = port;
    device->i2c_address = i2c_address;
    return 1;
}

uint8_t ads1115_read_single(ads1115_t *device, uint8_t channel,
                            ads1115_range_t range, int16_t *raw, int32_t *microvolts)
{
    uint8_t config[2];
    uint8_t data[2];
    uint16_t value;
    if (device == 0 || raw == 0 || microvolts == 0
        || channel > 3U || range > ADS1115_RANGE_0P256V) {
        return 0;
    }
    /* OS=1, MUX=channel, PGA=range, MODE=single, DR=128SPS, comparator off. */
    value = (uint16_t)(0x8000U
                       | ((uint16_t)channel << 12)
                       | ((uint16_t)range << 9)
                       | 0x0100U
                       | (0x04U << 5)
                       | 0x0003U);
    config[0] = (uint8_t)(value >> 8);
    config[1] = (uint8_t)value;
    if (!device->port.write(device->port.context, device->i2c_address,
                            REG_CONFIG, config, sizeof(config))) {
        return 0;
    }
    if (device->port.delay_ms != 0) {
        device->port.delay_ms(device->port.context, 10U);
    }
    if (!device->port.read(device->port.context, device->i2c_address,
                           REG_CONVERSION, data, sizeof(data))) {
        return 0;
    }
    *raw = (int16_t)(((uint16_t)data[0] << 8) | data[1]);
    *microvolts = (int32_t)((int64_t)*raw * full_scale_uv[range] / 32768);
    return 1;
}
