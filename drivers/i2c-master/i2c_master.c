#include "i2c_master.h"

static uint8_t timed_out(const i2c_master_t *master, uint32_t start)
{
    if (master->timeout_us == 0U) {
        return 0;
    }
    return (uint32_t)(master->port.now_us(master->port.context) - start) >= master->timeout_us;
}

static i2c_status_t write_byte(i2c_master_t *master, uint8_t byte, uint32_t start)
{
    if (!master->port.write(master->port.context, byte)) {
        return I2C_NACK;
    }
    return timed_out(master, start) ? I2C_TIMEOUT : I2C_OK;
}

static i2c_status_t read_byte(i2c_master_t *master, uint8_t *byte, uint8_t ack, uint32_t start)
{
    *byte = master->port.read(master->port.context, ack);
    return timed_out(master, start) ? I2C_TIMEOUT : I2C_OK;
}

void i2c_master_init(i2c_master_t *master, i2c_port_t port, uint32_t timeout_us)
{
    master->port = port;
    master->timeout_us = timeout_us;
}

i2c_status_t i2c_master_write(i2c_master_t *master, uint8_t address, const uint8_t *data, uint16_t length)
{
    uint32_t start;
    uint16_t index;
    i2c_status_t status;

    if (address > 0x7FU || (length != 0U && data == 0)) {
        return I2C_NACK;
    }
    start = master->port.now_us(master->port.context);
    master->port.start(master->port.context);
    status = write_byte(master, (uint8_t)(address << 1), start);
    for (index = 0; status == I2C_OK && index < length; index++) {
        status = write_byte(master, data[index], start);
    }
    if (status == I2C_OK && timed_out(master, start)) {
        status = I2C_TIMEOUT;
    }
    master->port.stop(master->port.context);
    return status;
}

i2c_status_t i2c_master_read(i2c_master_t *master, uint8_t address, uint8_t *data, uint16_t length)
{
    uint32_t start;
    uint16_t index;
    i2c_status_t status;

    if (address > 0x7FU || (length != 0U && data == 0)) {
        return I2C_NACK;
    }
    start = master->port.now_us(master->port.context);
    master->port.start(master->port.context);
    status = write_byte(master, (uint8_t)((address << 1) | 1U), start);
    for (index = 0; status == I2C_OK && index < length; index++) {
        status = read_byte(master, &data[index], (uint8_t)(index + 1U < length), start);
    }
    master->port.stop(master->port.context);
    return status;
}

i2c_status_t i2c_master_write_read(i2c_master_t *master, uint8_t address, const uint8_t *write, uint16_t write_length, uint8_t *read, uint16_t read_length)
{
    uint32_t start;
    uint16_t index;
    i2c_status_t status;

    if (address > 0x7FU
        || (write_length != 0U && write == 0)
        || (read_length != 0U && read == 0)) {
        return I2C_NACK;
    }
    start = master->port.now_us(master->port.context);
    master->port.start(master->port.context);
    status = write_byte(master, (uint8_t)(address << 1), start);
    for (index = 0; status == I2C_OK && index < write_length; index++) {
        status = write_byte(master, write[index], start);
    }
    if (status == I2C_OK) {
        master->port.start(master->port.context);
        status = write_byte(master, (uint8_t)((address << 1) | 1U), start);
    }
    for (index = 0; status == I2C_OK && index < read_length; index++) {
        status = read_byte(master, &read[index], (uint8_t)(index + 1U < read_length), start);
    }
    master->port.stop(master->port.context);
    return status;
}
