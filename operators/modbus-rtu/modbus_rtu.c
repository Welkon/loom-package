#include "modbus_rtu.h"

uint16_t modbus_crc16(const uint8_t *data, uint16_t length)
{
    uint16_t crc = 0xFFFFU;
    uint16_t i;
    uint8_t bit;
    if (data == 0) {
        return 0;
    }
    for (i = 0; i < length; i++) {
        crc ^= data[i];
        for (bit = 0; bit < 8U; bit++) {
            crc = (uint16_t)((crc & 1U) ? ((crc >> 1) ^ 0xA001U) : (crc >> 1));
        }
    }
    return crc;
}

static void append_crc(uint8_t *frame, uint16_t length)
{
    uint16_t crc = modbus_crc16(frame, length);
    frame[length] = (uint8_t)(crc & 0xFFU);
    frame[length + 1U] = (uint8_t)(crc >> 8);
}

uint8_t modbus_build_read(uint8_t slave, uint8_t function,
                          uint16_t address, uint16_t count, uint8_t *frame)
{
    if (frame == 0 || slave == 0U || count == 0U || count > 125U) {
        return 0;
    }
    frame[0] = slave;
    frame[1] = function;
    frame[2] = (uint8_t)(address >> 8);
    frame[3] = (uint8_t)address;
    frame[4] = (uint8_t)(count >> 8);
    frame[5] = (uint8_t)count;
    append_crc(frame, 6U);
    return 8;
}

uint8_t modbus_parse_read(const uint8_t *frame, uint16_t length,
                          uint8_t slave, uint8_t function,
                          uint16_t *values, uint8_t capacity, uint8_t *count)
{
    uint8_t bytes;
    uint8_t i;
    uint16_t crc;
    if (frame == 0 || values == 0 || count == 0
        || length < 5U || frame[0] != slave || frame[1] != function) {
        return 0;
    }
    crc = modbus_crc16(frame, (uint16_t)(length - 2U));
    if (frame[length - 2U] != (uint8_t)(crc & 0xFFU)
        || frame[length - 1U] != (uint8_t)(crc >> 8)) {
        return 0;
    }
    bytes = frame[2];
    if ((uint16_t)bytes + 5U != length || (bytes & 1U) != 0U
        || (uint8_t)(bytes / 2U) > capacity) {
        return 0;
    }
    for (i = 0; i < (uint8_t)(bytes / 2U); i++) {
        values[i] = (uint16_t)(((uint16_t)frame[3U + i * 2U] << 8)
                               | frame[4U + i * 2U]);
    }
    *count = (uint8_t)(bytes / 2U);
    return 1;
}

uint8_t modbus_parse_exception(const uint8_t *frame, uint16_t length,
                               uint8_t slave, uint8_t function, uint8_t *code)
{
    uint16_t crc;
    if (frame == 0 || code == 0 || length != 5U || frame[0] != slave
        || frame[1] != (uint8_t)(function | 0x80U)) {
        return 0;
    }
    crc = modbus_crc16(frame, 3U);
    if (frame[3] != (uint8_t)(crc & 0xFFU)
        || frame[4] != (uint8_t)(crc >> 8)) {
        return 0;
    }
    *code = frame[2];
    return 1;
}
