#include "rotary_encoder.h"

static const int8_t transitions[16] = {
     0,  1, -1,  0,
    -1,  0,  0,  1,
     1,  0,  0, -1,
     0, -1,  1,  0,
};

void encoder_init(encoder_t *encoder)
{
    encoder->state = 0;
    encoder->position = 0;
}

int8_t encoder_update(encoder_t *encoder, uint8_t a, uint8_t b)
{
    uint8_t current = (uint8_t)(((a & 1U) << 1) | (b & 1U));
    int8_t delta = transitions[(encoder->state << 2) | current];
    encoder->state = current;
    encoder->position = (int16_t)(encoder->position + delta);
    return delta;
}

int16_t encoder_position(const encoder_t *encoder)
{
    return encoder->position;
}
