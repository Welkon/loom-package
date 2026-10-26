#include "aht20.h"

static uint8_t crc8(const uint8_t *data, uint16_t length)
{
    uint8_t crc = 0xFFU;
    uint16_t i;
    uint8_t bit;
    for (i = 0; i < length; i++) {
        crc ^= data[i];
        for (bit = 0; bit < 8U; bit++) {
            if ((crc & 0x80U) != 0U) {
                crc = (uint8_t)((crc << 1) ^ 0x31U);
            } else {
                crc = (uint8_t)(crc << 1);
            }
        }
    }
    return crc;
}

uint8_t aht20_init(aht20_t *device, aht20_port_t port, uint8_t i2c_address)
{
    uint8_t status;
    uint8_t command[3] = {0xBEU, 0x08U, 0x00U};
    if (device == 0) {
        return 0;
    }
    device->port = port;
    device->i2c_address = i2c_address;
    if (port.delay_ms != 0) {
        port.delay_ms(port.context, 40U);
    }
    if (!port.read(port.context, i2c_address, &status, 1U)) {
        return 0;
    }
    if ((status & 0x18U) == 0x18U) {
        return 1;
    }
    if (!port.write(port.context, i2c_address, command, sizeof(command))) {
        return 0;
    }
    if (port.delay_ms != 0) {
        port.delay_ms(port.context, 10U);
    }
    if (!port.read(port.context, i2c_address, &status, 1U)) {
        return 0;
    }
    return (status & 0x18U) == 0x18U ? 1U : 0U;
}

uint8_t aht20_read(aht20_t *device, int32_t *milli_celsius, int32_t *milli_percent)
{
    uint8_t command[3] = {0xACU, 0x33U, 0x00U};
    uint8_t data[7];
    uint32_t raw_humidity;
    uint32_t raw_temperature;
    if (device == 0 || milli_celsius == 0 || milli_percent == 0) {
        return 0;
    }
    if (!device->port.write(device->port.context, device->i2c_address,
                            command, sizeof(command))) {
        return 0;
    }
    if (device->port.delay_ms != 0) {
        device->port.delay_ms(device->port.context, 80U);
    }
    if (!device->port.read(device->port.context, device->i2c_address,
                           data, sizeof(data))) {
        return 0;
    }
    if ((data[0] & 0x80U) != 0U || crc8(data, 6U) != data[6]) {
        return 0;
    }
    raw_humidity = ((uint32_t)data[1] << 16) | ((uint32_t)data[2] << 8) | data[3];
    raw_humidity >>= 4;
    raw_temperature = ((uint32_t)data[3] << 16) | ((uint32_t)data[4] << 8) | data[5];
    raw_temperature &= 0xFFFFFU;
    *milli_percent = (int32_t)((int64_t)raw_humidity * 100000 / 1048576);
    *milli_celsius = (int32_t)((int64_t)raw_temperature * 200000 / 1048576 - 50000);
    return 1;
}
