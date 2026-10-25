#include <stdio.h>
#include "ring_buffer.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

int main(void)
{
    uint8_t storage[3];
    ring_buffer_t buffer;
    uint8_t value = 0;

    ring_buffer_init(&buffer, storage, 3);
    CHECK(ring_buffer_empty(&buffer) == 1);
    CHECK(ring_buffer_push(&buffer, 1) == 1);
    CHECK(ring_buffer_push(&buffer, 2) == 1);
    CHECK(ring_buffer_push(&buffer, 3) == 1);
    CHECK(ring_buffer_push(&buffer, 4) == 0);
    CHECK(ring_buffer_full(&buffer) == 1);
    CHECK(ring_buffer_count(&buffer) == 3);

    CHECK(ring_buffer_pop(&buffer, &value) == 1 && value == 1);
    CHECK(ring_buffer_push(&buffer, 4) == 1);
    CHECK(ring_buffer_pop(&buffer, &value) == 1 && value == 2);
    CHECK(ring_buffer_pop(&buffer, &value) == 1 && value == 3);
    CHECK(ring_buffer_pop(&buffer, &value) == 1 && value == 4);
    CHECK(ring_buffer_pop(&buffer, &value) == 0);
    CHECK(ring_buffer_empty(&buffer) == 1);

    ring_buffer_init(&buffer, storage, 0);
    CHECK(ring_buffer_push(&buffer, 1) == 0);
    CHECK(ring_buffer_pop(&buffer, &value) == 0);

    if (failures) return 1;
    puts("ring_buffer: ok");
    return 0;
}
