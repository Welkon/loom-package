#include "ina219.h"

#define REG_CONFIG 0x00U
#define REG_SHUNT 0x01U
#define REG_BUS 0x02U
#define REG_POWER 0x03U
#define REG_CURRENT 0x04U
#define REG_CALIBRATION 0x05U

static uint8_t write_reg16(ina219_t *device, uint8_t reg, uint16_t value)
{
    uint8_t data[2] = {(uint8_t)(value >> 8), (uint8_t)value};
    return device->port.write(device->port.context, device->i2c_address,
                              reg, data, sizeof(data)) ? 1U : 0U;
}

static uint8_t read_reg16(ina219_t *device, uint8_t reg, uint16_t *value)
{
    uint8_t data[2];
    if (!device->port.read(device->port.context, device->i2c_address,
                           reg, data, sizeof(data))) {
        return 0;
    }
    *value = (uint16_t)(((uint16_t)data[0] << 8) | data[1]);
    return 1;
}

uint8_t ina219_init(ina219_t *device, ina219_port_t port, uint8_t i2c_address,
                    uint16_t shunt_milliohm, uint16_t current_lsb_ua)
{
    uint32_t calibration;
    if (device == 0 || shunt_milliohm == 0U || current_lsb_ua == 0U) {
        return 0;
    }
    device->port = port;
    device->i2c_address = i2c_address;
    device->current_lsb_ua = current_lsb_ua;
    /* 32 V range, +-320 mV PGA, 12-bit, continuous shunt and bus. */
    if (!write_reg16(device, REG_CONFIG, 0x399FU)) {
        return 0;
    }
    calibration = 40960000UL / ((uint32_t)current_lsb_ua * shunt_milliohm);
    if (calibration > 0xFFFFU) {
        calibration = 0xFFFFU;
    }
    return write_reg16(device, REG_CALIBRATION, (uint16_t)calibration);
}

uint8_t ina219_read_bus_voltage_mv(ina219_t *device, uint32_t *millivolts)
{
    uint16_t raw;
    if (device == 0 || millivolts == 0 || !read_reg16(device, REG_BUS, &raw)) {
        return 0;
    }
    *millivolts = (uint32_t)((raw >> 3) * 4U);
    return 1;
}

uint8_t ina219_read_shunt_voltage_uv(ina219_t *device, int32_t *microvolts)
{
    uint16_t raw;
    if (device == 0 || microvolts == 0 || !read_reg16(device, REG_SHUNT, &raw)) {
        return 0;
    }
    *microvolts = (int32_t)(int16_t)raw * 10;
    return 1;
}

uint8_t ina219_read_current_ua(ina219_t *device, int32_t *microamps)
{
    uint16_t raw;
    if (device == 0 || microamps == 0 || !read_reg16(device, REG_CURRENT, &raw)) {
        return 0;
    }
    *microamps = (int32_t)(int16_t)raw * device->current_lsb_ua;
    return 1;
}

uint8_t ina219_read_power_mw(ina219_t *device, uint32_t *milliwatts)
{
    uint16_t raw;
    if (device == 0 || milliwatts == 0 || !read_reg16(device, REG_POWER, &raw)) {
        return 0;
    }
    *milliwatts = (uint32_t)(((uint32_t)raw * 20U * device->current_lsb_ua) / 1000U);
    return 1;
}
