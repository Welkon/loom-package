# ring-buffer

Fixed-size byte ring buffer with explicit full and empty results.

## Contract

```c
void ring_buffer_init(ring_buffer_t *buffer, uint8_t *data, uint16_t size);
uint8_t ring_buffer_push(ring_buffer_t *buffer, uint8_t value);
uint8_t ring_buffer_pop(ring_buffer_t *buffer, uint8_t *value);
uint16_t ring_buffer_count(const ring_buffer_t *buffer);
uint8_t ring_buffer_full(const ring_buffer_t *buffer);
uint8_t ring_buffer_empty(const ring_buffer_t *buffer);
void ring_buffer_clear(ring_buffer_t *buffer);
```

- The caller owns the storage; `size` must be at least 1.
- `push` returns 0 when full; `pop` returns 0 when empty.
- Single producer and single consumer are safe when each side owns one index.
- No allocation.

## Use

Use it for UART receive queues, IR edge queues, and event buffers.

## Avoid

Do not use it for multi-producer writes without a lock or a separate
critical section. Do not use it for variable-size records.
