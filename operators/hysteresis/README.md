# hysteresis

Keep a binary state while the input stays inside a band.

## Contract

```c
void hysteresis_init(hysteresis_t *filter, int32_t low, int32_t high, uint8_t initial);
uint8_t hysteresis_step(hysteresis_t *filter, int32_t value);
```

- `low < high`; values at or above `high` set the state.
- Values at or below `low` clear it.
- Values inside the band keep the previous state.
- No allocation.

## Use

Use it for battery thresholds, presence, and any noisy boundary. Behavior
reference: a production target `drivers.c` and a production target `control.c`; this implementation
is original.

## Avoid

Do not use it when the band must be crossed only after N samples; add
`debounce` after the hysteresis.
