#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t *data;
    uint16_t size;
    uint16_t read;
    uint16_t write;
    uint16_t count;
} ring_buffer_t;

void ring_buffer_init(ring_buffer_t *buffer, uint8_t *data, uint16_t size);
uint8_t ring_buffer_push(ring_buffer_t *buffer, uint8_t value);
uint8_t ring_buffer_pop(ring_buffer_t *buffer, uint8_t *value);
uint16_t ring_buffer_count(const ring_buffer_t *buffer);
uint8_t ring_buffer_full(const ring_buffer_t *buffer);
uint8_t ring_buffer_empty(const ring_buffer_t *buffer);
void ring_buffer_clear(ring_buffer_t *buffer);

#ifdef __cplusplus
}
#endif

#endif
