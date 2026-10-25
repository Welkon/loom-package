#ifndef FIR_H
#define FIR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const int16_t *taps;
    int16_t *history;
    uint16_t length;
    uint16_t index;
    uint8_t shift;
} fir_t;

void fir_init(fir_t *filter, const int16_t *taps, int16_t *history,
              uint16_t length, uint8_t shift);
int32_t fir_step(fir_t *filter, int16_t sample);
void fir_reset(fir_t *filter);

#ifdef __cplusplus
}
#endif

#endif
