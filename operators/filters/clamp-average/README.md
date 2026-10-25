# clamp-average

Limit each step, then smooth with a moving average.

## Contract

```c
void clamp_average_init(clamp_average_t *filter, uint16_t initial, uint16_t max_delta, uint16_t *window, uint8_t size);
uint16_t clamp_average_step(clamp_average_t *filter, uint16_t sample);
```

- `max_delta` caps the change from the previous clamped value.
- The clamped value enters a moving-average window.
- The caller owns `window`; `size` must be 1..255.
- No allocation.

## Use

Use it when a sensor can jump but the output also needs smoothing.
Behavior reference: Padauk `Soft_Filter/AD_filter_06`; this implementation
is original.

## Avoid

Do not use it when a real step change must pass immediately; clamp will
delay it. Use `mean` or `moving-average` without clamp.
