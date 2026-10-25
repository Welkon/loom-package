#ifndef WEIGHTED_AVERAGE_H
#define WEIGHTED_AVERAGE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint16_t weighted_mean_u16(const uint16_t *samples, const uint8_t *weights, uint8_t count);

#ifdef __cplusplus
}
#endif

#endif
