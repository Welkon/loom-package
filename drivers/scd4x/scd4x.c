#include "scd4x.h"

#define CMD_START 0x21B1U
#define CMD_READ 0xEC05U
#define CMD_STOP 0x3F86U
#define CMD_DATA_READY 0xE4B8U

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

static uint8_t command(scd4x_t *sensor, uint16_t value)
{
    uint8_t data[2] = {(uint8_t)(value >> 8), (uint8_t)value};
    return sensor->port.write(sensor->port.context, sensor->i2c_address,
                              data, sizeof(data)) ? 1U : 0U;
}

uint8_t scd4x_init(scd4x_t *sensor, scd4x_port_t port, uint8_t i2c_address)
{
    if (sensor == 0) {
        return 0;
    }
    sensor->port = port;
    sensor->i2c_address = i2c_address;
    if (!command(sensor, CMD_STOP)) {
        return 0;
    }
    if (port.delay_ms != 0) {
        port.delay_ms(port.context, 500U);
    }
    return command(sensor, CMD_START);
}

uint8_t scd4x_read(scd4x_t *sensor, uint16_t *co2_ppm,
                   int32_t *milli_celsius, uint32_t *milli_percent)
{
    uint8_t data[9];
    uint16_t status = 0;
    uint16_t wait;
    if (sensor == 0 || co2_ppm == 0 || milli_celsius == 0 || milli_percent == 0) {
        return 0;
    }
    for (wait = 0; wait < 50U; wait++) {
        if (!command(sensor, CMD_DATA_READY)
            || !sensor->port.read(sensor->port.context, sensor->i2c_address, data, 3U)) {
            return 0;
        }
        status = (uint16_t)(((uint16_t)data[0] << 8) | data[1]);
        if ((status & 0x07FFU) != 0U) {
            break;
        }
        if (sensor->port.delay_ms != 0) {
            sensor->port.delay_ms(sensor->port.context, 100U);
        }
    }
    if (wait == 50U || !command(sensor, CMD_READ)
        || !sensor->port.read(sensor->port.context, sensor->i2c_address, data, sizeof(data))) {
        return 0;
    }
    if (crc8(&data[0], 2U) != data[2]
        || crc8(&data[3], 2U) != data[5]
        || crc8(&data[6], 2U) != data[8]) {
        return 0;
    }
    *co2_ppm = (uint16_t)(((uint16_t)data[0] << 8) | data[1]);
    *milli_celsius = (int32_t)(((int64_t)(((uint16_t)data[3] << 8) | data[4]) * 175000) / 65536 - 45000);
    *milli_percent = (uint32_t)(((uint64_t)(((uint16_t)data[6] << 8) | data[7]) * 100000U) / 65536U);
    return 1;
}
