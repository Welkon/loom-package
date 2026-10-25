#ifndef GAMMA_LUT_H
#define GAMMA_LUT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint16_t gamma_lut_apply(const uint16_t *table, uint8_t entries,
                         uint16_t input, uint16_t input_max);

#ifdef __cplusplus
}
#endif

#endif
