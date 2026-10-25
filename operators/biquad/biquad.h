#ifndef BIQUAD_H
#define BIQUAD_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int32_t b0;
    int32_t b1;
    int32_t b2;
    int32_t a1;
    int32_t a2;
    int32_t x1;
    int32_t x2;
    int32_t y1;
    int32_t y2;
    uint8_t shift;
} biquad_t;

void biquad_init(biquad_t *filter, int32_t b0, int32_t b1, int32_t b2,
                 int32_t a1, int32_t a2, uint8_t shift);
int32_t biquad_step(biquad_t *filter, int32_t sample);
void biquad_reset(biquad_t *filter);

#ifdef __cplusplus
}
#endif

#endif
