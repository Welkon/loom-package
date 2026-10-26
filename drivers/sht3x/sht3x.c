#include "sht3x.h"

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

uint8_t sht3x_init(sht3x_t *device, sht3x_port_t port, uint8_t i2c_address)
{
    uint8_t command[2] = {0x30U, 0xA2U};
    if (device == 0) {
        return 0;
    }
    device->port = port;
    device->i2c_address = i2c_address;
    if (!port.write(port.context, i2c_address, command, sizeof(command))) {
        return 0;
    }
    if (port.delay_ms != 0) {
        port.delay_ms(port.context, 2U);
    }
    return 1;
}

uint8_t sht3x_read(sht3x_t *device, sht3x_repeatability_t repeatability,
                   int32_t *milli_celsius, int32_t *milli_percent)
{
    uint8_t command[2];
    uint8_t data[6];
    uint16_t raw_temperature;
    uint16_t raw_humidity;
    uint16_t delay_ms;
    if (device == 0 || milli_celsius == 0 || milli_percent == 0) {
        return 0;
    }
    if (repeatability == SHT3X_MEDIUM) {
        command[0] = 0x24U; command[1] = 0x0BU; delay_ms = 6U;
    } else if (repeatability == SHT3X_LOW) {
        command[0] = 0x24U; command[1] = 0x16U; delay_ms = 4U;
    } else {
        command[0] = 0x24U; command[1] = 0x00U; delay_ms = 15U;
    }
    if (!device->port.write(device->port.context, device->i2c_address,
                            command, sizeof(command))) {
        return 0;
    }
    if (device->port.delay_ms != 0) {
        device->port.delay_ms(device->port.context, delay_ms);
    }
    if (!device->port.read(device->port.context, device->i2c_address,
                           data, sizeof(data))) {
        return 0;
    }
    if (crc8(&data[0], 2U) != data[2] || crc8(&data[3], 2U) != data[5]) {
        return 0;
    }
    raw_temperature = (uint16_t)(((uint16_t)data[0] << 8) | data[1]);
    raw_humidity = (uint16_t)(((uint16_t)data[3] << 8) | data[4]);
    *milli_celsius = (int32_t)(-45000 + (int64_t)175000 * raw_temperature / 65535);
    *milli_percent = (int32_t)((int64_t)100000 * raw_humidity / 65535);
    return 1;
}
