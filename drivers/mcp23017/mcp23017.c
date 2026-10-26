#include "mcp23017.h"

#define REG_IODIR_A 0x00U
#define REG_IODIR_B 0x01U
#define REG_IOCON 0x0AU
#define REG_GPPU_A 0x0CU
#define REG_GPPU_B 0x0DU
#define REG_GPIO_A 0x12U
#define REG_GPIO_B 0x13U
#define REG_OLAT_A 0x14U
#define REG_OLAT_B 0x15U

static uint8_t write_reg(mcp23017_t *device, uint8_t reg, uint8_t value)
{
    return device->port.write(device->port.context, device->i2c_address,
                              reg, &value, 1U) ? 1U : 0U;
}

static uint8_t read_reg(mcp23017_t *device, uint8_t reg, uint8_t *value)
{
    return device->port.read(device->port.context, device->i2c_address,
                             reg, value, 1U) ? 1U : 0U;
}

static uint8_t update_bit(mcp23017_t *device, uint8_t reg, uint8_t bit, uint8_t value)
{
    uint8_t current;
    if (!read_reg(device, reg, &current)) {
        return 0;
    }
    if (value) {
        current = (uint8_t)(current | (1U << bit));
    } else {
        current = (uint8_t)(current & (uint8_t)~(1U << bit));
    }
    return write_reg(device, reg, current);
}

uint8_t mcp23017_init(mcp23017_t *device, mcp23017_port_t port, uint8_t i2c_address)
{
    if (device == 0) {
        return 0;
    }
    device->port = port;
    device->i2c_address = i2c_address;
    return write_reg(device, REG_IOCON, 0x00U)
        && write_reg(device, REG_IODIR_A, 0xFFU)
        && write_reg(device, REG_IODIR_B, 0xFFU)
        && write_reg(device, REG_GPPU_A, 0x00U)
        && write_reg(device, REG_GPPU_B, 0x00U)
        && write_reg(device, REG_OLAT_A, 0x00U)
        && write_reg(device, REG_OLAT_B, 0x00U);
}

uint8_t mcp23017_pin_mode(mcp23017_t *device, uint8_t pin, uint8_t output)
{
    if (device == 0 || pin > 15U) {
        return 0;
    }
    return update_bit(device, pin < 8U ? REG_IODIR_A : REG_IODIR_B,
                      (uint8_t)(pin % 8U), output ? 0U : 1U);
}

uint8_t mcp23017_write_pin(mcp23017_t *device, uint8_t pin, uint8_t value)
{
    if (device == 0 || pin > 15U) {
        return 0;
    }
    return update_bit(device, pin < 8U ? REG_OLAT_A : REG_OLAT_B,
                      (uint8_t)(pin % 8U), value ? 1U : 0U);
}

uint8_t mcp23017_read_pin(mcp23017_t *device, uint8_t pin, uint8_t *value)
{
    uint8_t port_value;
    if (device == 0 || value == 0 || pin > 15U
        || !read_reg(device, pin < 8U ? REG_GPIO_A : REG_GPIO_B, &port_value)) {
        return 0;
    }
    *value = (uint8_t)((port_value >> (pin % 8U)) & 1U);
    return 1;
}

uint8_t mcp23017_write_port(mcp23017_t *device, uint8_t port, uint8_t value)
{
    if (device == 0 || port > 1U) {
        return 0;
    }
    return write_reg(device, port == 0U ? REG_OLAT_A : REG_OLAT_B, value);
}

uint8_t mcp23017_read_port(mcp23017_t *device, uint8_t port, uint8_t *value)
{
    if (device == 0 || value == 0 || port > 1U) {
        return 0;
    }
    return read_reg(device, port == 0U ? REG_GPIO_A : REG_GPIO_B, value);
}

uint8_t mcp23017_set_pullup(mcp23017_t *device, uint8_t pin, uint8_t enable)
{
    if (device == 0 || pin > 15U) {
        return 0;
    }
    return update_bit(device, pin < 8U ? REG_GPPU_A : REG_GPPU_B,
                      (uint8_t)(pin % 8U), enable ? 1U : 0U);
}
