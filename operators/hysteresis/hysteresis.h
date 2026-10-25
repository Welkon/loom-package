#ifndef HYSTERESIS_H
#define HYSTERESIS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int32_t low;
    int32_t high;
    uint8_t state;
} hysteresis_t;

void hysteresis_init(hysteresis_t *filter, int32_t low, int32_t high, uint8_t initial);
uint8_t hysteresis_step(hysteresis_t *filter, int32_t value);

#ifdef __cplusplus
}
#endif

#endif
