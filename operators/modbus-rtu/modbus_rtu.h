#ifndef MODBUS_RTU_H
#define MODBUS_RTU_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint16_t modbus_crc16(const uint8_t *data, uint16_t length);
uint8_t modbus_build_read(uint8_t slave, uint8_t function,
                          uint16_t address, uint16_t count, uint8_t *frame);
uint8_t modbus_parse_read(const uint8_t *frame, uint16_t length,
                          uint8_t slave, uint8_t function,
                          uint16_t *values, uint8_t capacity, uint8_t *count);
uint8_t modbus_parse_exception(const uint8_t *frame, uint16_t length,
                               uint8_t slave, uint8_t function, uint8_t *code);

#ifdef __cplusplus
}
#endif

#endif
