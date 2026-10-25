# ring-buffer

Fixed-size byte ring buffer with explicit full and empty results.

## Application scenarios

- A UART receive ISR pushes bytes and the main loop pops them.
- An IR capture queue stores edge timestamps before decoding.
- A logger keeps the last N events without allocation.

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

- Caller owns `data`; `size` must be at least 1.
- `push` returns 0 when full; `pop` returns 0 when empty.
- Drop-newest on full; the caller can choose a different policy.
- No allocation.

## Parameters

- `size`: worst-case burst plus main-loop latency.
- Memory is `2*size` bytes plus 12 bytes of state.

## Memory and timing

- O(1) push/pop; no locks.
- Single producer and single consumer are safe when each owns one index.

## Integration

```c
uint8_t storage[64];
ring_buffer_t rx;
ring_buffer_init(&rx, storage, sizeof(storage));
/* ISR: */ ring_buffer_push(&rx, byte);
/* main: */ while (ring_buffer_pop(&rx, &byte)) { handle(byte); }
```

## When not to use

- Multiple producers without a critical section.
- Variable-size records; use a byte stream plus a frame parser.

## Verification

`make test` covers wrap, full, empty, count, clear, and size 0.
