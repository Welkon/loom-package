#include "crc32.h"

uint32_t crc32_update(uint32_t crc, uint8_t byte)
{
    uint8_t bit;
    crc ^= byte;
    for (bit = 0; bit < 8U; bit++) {
        uint32_t mask = (uint32_t)-(int32_t)(crc & 1U);
        crc = (crc >> 1) ^ (0xEDB88320U & mask);
    }
    return crc;
}

uint32_t crc32(const uint8_t *data, size_t length)
{
    uint32_t crc = 0xFFFFFFFFU;
    size_t index;
    for (index = 0; index < length; index++) {
        crc = crc32_update(crc, data[index]);
    }
    return crc ^ 0xFFFFFFFFU;
}
