#include <stdio.h>
#include "rotary_encoder.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

int main(void)
{
    encoder_t encoder;

    encoder_init(&encoder);
    CHECK(encoder_update(&encoder, 0, 1) == 1);
    CHECK(encoder_update(&encoder, 1, 1) == 1);
    CHECK(encoder_update(&encoder, 1, 0) == 1);
    CHECK(encoder_update(&encoder, 0, 0) == 1);
    CHECK(encoder_position(&encoder) == 4);

    encoder_init(&encoder);
    CHECK(encoder_update(&encoder, 1, 1) == 0);
    CHECK(encoder_update(&encoder, 0, 0) == 0);
    CHECK(encoder_position(&encoder) == 0);

    encoder_init(&encoder);
    CHECK(encoder_update(&encoder, 0, 1) == 1);
    CHECK(encoder_update(&encoder, 0, 1) == 0);
    CHECK(encoder_update(&encoder, 0, 0) == -1);
    CHECK(encoder_update(&encoder, 1, 1) == 0);
    CHECK(encoder_position(&encoder) == 0);

    if (failures) return 1;
    puts("rotary_encoder: ok");
    return 0;
}
