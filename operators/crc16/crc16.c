#include "crc16.h"

uint16_t crc16_ccitt_update(uint16_t crc, uint8_t byte)
{
    uint8_t bit;
    crc ^= (uint16_t)byte << 8;
    for (bit = 0; bit < 8U; bit++) {
        crc = (crc & 0x8000U) ? (uint16_t)((crc << 1) ^ 0x1021U) : (uint16_t)(crc << 1);
    }
    return crc;
}

uint16_t crc16_ccitt(const uint8_t *data, size_t length)
{
    uint16_t crc = 0xFFFFU;
    size_t index;
    for (index = 0; index < length; index++) {
        crc = crc16_ccitt_update(crc, data[index]);
    }
    return crc;
}
