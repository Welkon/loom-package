#ifndef INA219_H
#define INA219_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *context;
    uint8_t (*write)(void *context, uint8_t address, uint8_t reg,
                     const uint8_t *data, uint16_t length);
    uint8_t (*read)(void *context, uint8_t address, uint8_t reg,
                    uint8_t *data, uint16_t length);
} ina219_port_t;

typedef struct {
    ina219_port_t port;
    uint8_t i2c_address;
    uint16_t current_lsb_ua;
} ina219_t;

uint8_t ina219_init(ina219_t *device, ina219_port_t port, uint8_t i2c_address,
                    uint16_t shunt_milliohm, uint16_t current_lsb_ua);
uint8_t ina219_read_bus_voltage_mv(ina219_t *device, uint32_t *millivolts);
uint8_t ina219_read_shunt_voltage_uv(ina219_t *device, int32_t *microvolts);
uint8_t ina219_read_current_ua(ina219_t *device, int32_t *microamps);
uint8_t ina219_read_power_mw(ina219_t *device, uint32_t *milliwatts);

#ifdef __cplusplus
}
#endif

#endif
