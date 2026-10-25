#ifndef TRIMMED_MEAN_H
#define TRIMMED_MEAN_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint16_t trimmed_mean(const uint16_t *samples, uint8_t count);

#ifdef __cplusplus
}
#endif

#endif
