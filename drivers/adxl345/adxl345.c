#include "adxl345.h"

#define REG_DEVID 0x00U
#define REG_BW_RATE 0x2CU
#define REG_POWER_CTL 0x2DU
#define REG_DATA_FORMAT 0x31U
#define REG_DATAX0 0x32U

uint8_t adxl345_init(adxl345_t *device, adxl345_port_t port, uint8_t i2c_address)
{
    uint8_t identity;
    uint8_t value;
    if (device == 0) {
        return 0;
    }
    device->port = port;
    device->i2c_address = i2c_address;
    if (!port.read(port.context, i2c_address, REG_DEVID, &identity, 1U)
        || identity != 0xE5U) {
        return 0;
    }
    value = 0x0AU; /* 100 Hz */
    if (!port.write(port.context, i2c_address, REG_BW_RATE, &value, 1U)) {
        return 0;
    }
    value = 0x0BU; /* full resolution, +-16 g */
    if (!port.write(port.context, i2c_address, REG_DATA_FORMAT, &value, 1U)) {
        return 0;
    }
    value = 0x08U; /* measure */
    if (!port.write(port.context, i2c_address, REG_POWER_CTL, &value, 1U)) {
        return 0;
    }
    if (port.delay_ms != 0) {
        port.delay_ms(port.context, 10U);
    }
    return 1;
}

uint8_t adxl345_read(adxl345_t *device, adxl345_reading_t *reading)
{
    uint8_t data[6];
    if (device == 0 || reading == 0
        || !device->port.read(device->port.context, device->i2c_address,
                              REG_DATAX0, data, sizeof(data))) {
        return 0;
    }
    reading->x = (int16_t)(((uint16_t)data[1] << 8) | data[0]);
    reading->y = (int16_t)(((uint16_t)data[3] << 8) | data[2]);
    reading->z = (int16_t)(((uint16_t)data[5] << 8) | data[4]);
    reading->x_mg = (int32_t)reading->x * 39 / 10;
    reading->y_mg = (int32_t)reading->y * 39 / 10;
    reading->z_mg = (int32_t)reading->z * 39 / 10;
    return 1;
}
