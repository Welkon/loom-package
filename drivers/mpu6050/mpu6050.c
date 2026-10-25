#include "mpu6050.h"

#define REG_SMPLRT_DIV 0x19U
#define REG_CONFIG 0x1AU
#define REG_GYRO_CONFIG 0x1BU
#define REG_ACCEL_CONFIG 0x1CU
#define REG_ACCEL_XOUT_H 0x3BU
#define REG_PWR_MGMT_1 0x6BU
#define REG_WHO_AM_I 0x75U

static uint8_t write_reg(mpu6050_t *device, uint8_t reg, uint8_t value)
{
    uint8_t data[2] = {reg, value};
    return device->port.write(device->port.context, device->i2c_address,
                              data, sizeof(data)) ? 1U : 0U;
}

static uint8_t read_regs(mpu6050_t *device, uint8_t reg, uint8_t *data, uint16_t length)
{
    if (!device->port.write(device->port.context, device->i2c_address, &reg, 1U)) {
        return 0;
    }
    return device->port.read(device->port.context, device->i2c_address, data, length) ? 1U : 0U;
}

uint8_t mpu6050_init(mpu6050_t *device, mpu6050_port_t port, uint8_t i2c_address)
{
    uint8_t identity;
    if (device == 0) {
        return 0;
    }
    device->port = port;
    device->i2c_address = i2c_address;
    if (!read_regs(device, REG_WHO_AM_I, &identity, 1U) || identity != 0x68U) {
        return 0;
    }
    if (!write_reg(device, REG_PWR_MGMT_1, 0x80U)) {
        return 0;
    }
    if (device->port.delay_ms != 0) {
        device->port.delay_ms(device->port.context, 100U);
    }
    return write_reg(device, REG_PWR_MGMT_1, 0x01U)
        && write_reg(device, REG_SMPLRT_DIV, 0x07U)
        && write_reg(device, REG_CONFIG, 0x03U)
        && write_reg(device, REG_GYRO_CONFIG, 0x00U)
        && write_reg(device, REG_ACCEL_CONFIG, 0x00U);
}

uint8_t mpu6050_read_raw(mpu6050_t *device, mpu6050_raw_t *raw)
{
    uint8_t data[14];
    if (device == 0 || raw == 0
        || !read_regs(device, REG_ACCEL_XOUT_H, data, sizeof(data))) {
        return 0;
    }
    raw->accel_x = (int16_t)(((uint16_t)data[0] << 8) | data[1]);
    raw->accel_y = (int16_t)(((uint16_t)data[2] << 8) | data[3]);
    raw->accel_z = (int16_t)(((uint16_t)data[4] << 8) | data[5]);
    raw->temperature = (int16_t)(((uint16_t)data[6] << 8) | data[7]);
    raw->gyro_x = (int16_t)(((uint16_t)data[8] << 8) | data[9]);
    raw->gyro_y = (int16_t)(((uint16_t)data[10] << 8) | data[11]);
    raw->gyro_z = (int16_t)(((uint16_t)data[12] << 8) | data[13]);
    return 1;
}

uint8_t mpu6050_read_scaled(mpu6050_t *device, mpu6050_scaled_t *scaled)
{
    mpu6050_raw_t raw;
    if (scaled == 0 || !mpu6050_read_raw(device, &raw)) {
        return 0;
    }
    scaled->accel_x_mg = (int32_t)((int64_t)raw.accel_x * 1000 / 16384);
    scaled->accel_y_mg = (int32_t)((int64_t)raw.accel_y * 1000 / 16384);
    scaled->accel_z_mg = (int32_t)((int64_t)raw.accel_z * 1000 / 16384);
    scaled->gyro_x_mdps = (int32_t)((int64_t)raw.gyro_x * 250000 / 32768);
    scaled->gyro_y_mdps = (int32_t)((int64_t)raw.gyro_y * 250000 / 32768);
    scaled->gyro_z_mdps = (int32_t)((int64_t)raw.gyro_z * 250000 / 32768);
    scaled->temperature_milli_celsius =
        (int32_t)((int64_t)raw.temperature * 1000 / 340 + 36530);
    return 1;
}
