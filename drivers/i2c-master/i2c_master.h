#ifndef I2C_MASTER_H
#define I2C_MASTER_H

#include "port.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    I2C_OK = 0,
    I2C_NACK = 1,
    I2C_TIMEOUT = 2
} i2c_status_t;

typedef struct {
    i2c_port_t port;
    uint32_t timeout_us;
} i2c_master_t;

void i2c_master_init(i2c_master_t *master, i2c_port_t port, uint32_t timeout_us);
i2c_status_t i2c_master_write(i2c_master_t *master, uint8_t address, const uint8_t *data, uint16_t length);
i2c_status_t i2c_master_read(i2c_master_t *master, uint8_t address, uint8_t *data, uint16_t length);
i2c_status_t i2c_master_write_read(i2c_master_t *master, uint8_t address, const uint8_t *write, uint16_t write_length, uint8_t *read, uint16_t read_length);

#ifdef __cplusplus
}
#endif

#endif
