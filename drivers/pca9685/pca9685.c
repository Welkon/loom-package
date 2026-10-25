#include "pca9685.h"

#define MODE1 0x00U
#define PRE_SCALE 0xFEU

static uint8_t write_regs(pca9685_t *device, uint8_t reg, const uint8_t *data, uint16_t length)
{
    uint8_t buffer[5];
    uint16_t i;
    if (length > sizeof(buffer) - 1U) {
        return 0;
    }
    buffer[0] = reg;
    for (i = 0; i < length; i++) {
        buffer[i + 1U] = data[i];
    }
    return device->port.write(device->port.context, device->i2c_address,
                              buffer, (uint16_t)(length + 1U)) ? 1U : 0U;
}

static uint8_t read_regs(pca9685_t *device, uint8_t reg, uint8_t *data, uint16_t length)
{
    if (!device->port.write(device->port.context, device->i2c_address, &reg, 1U)) {
        return 0;
    }
    return device->port.read(device->port.context, device->i2c_address, data, length) ? 1U : 0U;
}

static uint8_t write_reg(pca9685_t *device, uint8_t reg, uint8_t value)
{
    return write_regs(device, reg, &value, 1U);
}

static uint8_t prescaler_for(uint16_t frequency_hz, uint8_t *prescaler)
{
    uint32_t denominator;
    uint32_t rounded;
    if (frequency_hz == 0U) {
        return 0;
    }
    denominator = 4096UL * frequency_hz;
    rounded = (25000000UL + denominator / 2UL) / denominator;
    if (rounded == 0U) {
        rounded = 1U;
    }
    if (rounded > 256U) {
        rounded = 256U;
    }
    *prescaler = (uint8_t)(rounded - 1U);
    return 1;
}

static uint8_t apply_frequency(pca9685_t *device, uint16_t frequency_hz)
{
    uint8_t mode1;
    uint8_t prescaler;
    if (!prescaler_for(frequency_hz, &prescaler)
        || !read_regs(device, MODE1, &mode1, 1U)) {
        return 0;
    }
    if (!write_reg(device, MODE1, (uint8_t)((mode1 & ~0x10U) | 0x10U | 0x20U))) {
        return 0;
    }
    if (!write_reg(device, PRE_SCALE, prescaler)) {
        return 0;
    }
    if (!write_reg(device, MODE1, (uint8_t)((mode1 & ~0x10U) | 0x20U))) {
        return 0;
    }
    if (device->port.delay_ms != 0) {
        device->port.delay_ms(device->port.context, 1U);
    }
    return write_reg(device, MODE1, (uint8_t)((mode1 & ~0x10U) | 0x20U | 0x80U));
}

uint8_t pca9685_init(pca9685_t *device, pca9685_port_t port,
                     uint8_t i2c_address, uint16_t frequency_hz)
{
    if (device == 0) {
        return 0;
    }
    device->port = port;
    device->i2c_address = i2c_address;
    return apply_frequency(device, frequency_hz);
}

uint8_t pca9685_set_pwm(pca9685_t *device, uint8_t channel,
                        uint16_t on_count, uint16_t off_count)
{
    uint8_t data[4];
    if (device == 0 || channel > 15U || on_count > 4095U || off_count > 4095U) {
        return 0;
    }
    data[0] = (uint8_t)(on_count & 0xFFU);
    data[1] = (uint8_t)(on_count >> 8);
    data[2] = (uint8_t)(off_count & 0xFFU);
    data[3] = (uint8_t)(off_count >> 8);
    return write_regs(device, (uint8_t)(0x06U + 4U * channel), data, sizeof(data));
}

uint8_t pca9685_set_all(pca9685_t *device, uint16_t on_count, uint16_t off_count)
{
    uint8_t data[4];
    if (device == 0 || on_count > 4095U || off_count > 4095U) {
        return 0;
    }
    data[0] = (uint8_t)(on_count & 0xFFU);
    data[1] = (uint8_t)(on_count >> 8);
    data[2] = (uint8_t)(off_count & 0xFFU);
    data[3] = (uint8_t)(off_count >> 8);
    return write_regs(device, 0xFAU, data, sizeof(data));
}

uint8_t pca9685_set_frequency(pca9685_t *device, uint16_t frequency_hz)
{
    return device != 0 ? apply_frequency(device, frequency_hz) : 0;
}
