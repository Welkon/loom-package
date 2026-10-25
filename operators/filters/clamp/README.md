# clamp

Limit how far one output step may move from the previous output.

## Contract

```c
void clamp_filter_init(clamp_filter_t *filter, int32_t initial, uint16_t max_delta);
int32_t clamp_filter_step(clamp_filter_t *filter, int32_t sample);
void clamp_filter_reset(clamp_filter_t *filter, int32_t value);
```

- `max_delta` is the largest allowed change per step.
- State is one `int32_t` plus the limit; no allocation.
- A delta larger than the limit is clamped, not rejected.

## Use

Use it when a sensor can jump because of a bad sample but the signal is
otherwise continuous. It does not remove a persistent offset.

## Avoid

Do not use it to hide a real step change. Use `median` for impulses and
`moving-average` or `ema` for noise.
