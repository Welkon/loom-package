#include "ring_buffer.h"

void ring_buffer_init(ring_buffer_t *buffer, uint8_t *data, uint16_t size)
{
    buffer->data = data;
    buffer->size = size;
    buffer->read = 0;
    buffer->write = 0;
    buffer->count = 0;
}

uint8_t ring_buffer_push(ring_buffer_t *buffer, uint8_t value)
{
    if (buffer->size == 0U || buffer->count >= buffer->size) {
        return 0;
    }
    buffer->data[buffer->write] = value;
    buffer->write = (uint16_t)((buffer->write + 1U) % buffer->size);
    buffer->count++;
    return 1;
}

uint8_t ring_buffer_pop(ring_buffer_t *buffer, uint8_t *value)
{
    if (buffer->count == 0U) {
        return 0;
    }
    *value = buffer->data[buffer->read];
    buffer->read = (uint16_t)((buffer->read + 1U) % buffer->size);
    buffer->count--;
    return 1;
}

uint16_t ring_buffer_count(const ring_buffer_t *buffer)
{
    return buffer->count;
}

uint8_t ring_buffer_full(const ring_buffer_t *buffer)
{
    return (uint8_t)(buffer->size != 0U && buffer->count >= buffer->size);
}

uint8_t ring_buffer_empty(const ring_buffer_t *buffer)
{
    return (uint8_t)(buffer->count == 0U);
}

void ring_buffer_clear(ring_buffer_t *buffer)
{
    buffer->read = 0;
    buffer->write = 0;
    buffer->count = 0;
}
