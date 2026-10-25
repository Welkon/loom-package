#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t state;
    int16_t position;
} encoder_t;

void encoder_init(encoder_t *encoder);
int8_t encoder_update(encoder_t *encoder, uint8_t a, uint8_t b);
int16_t encoder_position(const encoder_t *encoder);

#ifdef __cplusplus
}
#endif

#endif
