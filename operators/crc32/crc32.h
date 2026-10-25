#ifndef CRC32_H
#define CRC32_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t crc32_update(uint32_t crc, uint8_t byte);
uint32_t crc32(const uint8_t *data, size_t length);

#ifdef __cplusplus
}
#endif

#endif
