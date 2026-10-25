# median

Return the median of the last N samples.

## Contract

```c
void median_filter_init(median_filter_t *filter, uint16_t *window, uint8_t size);
uint16_t median_filter_step(median_filter_t *filter, uint16_t sample);
void median_filter_reset(median_filter_t *filter);
```

- The caller supplies `window`, an array of `size` elements.
- `size` must be 1..255. Odd sizes give the true median; even sizes
  return the lower middle value.
- The implementation uses no scratch buffer and no allocation.
- Cost is O(N^2) per sample. Use it for small N, typically 3, 5, or 7.

## Use

Use it when isolated impulses or spikes must not reach the output.

## Avoid

Do not use it for a fast control loop with a large window; the O(N^2) scan
and the delay may be too high. Use `moving-average` or `ema` instead.
