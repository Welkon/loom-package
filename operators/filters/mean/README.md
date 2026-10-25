# mean

Arithmetic mean of a batch.

## Contract

```c
uint16_t mean_u16(const uint16_t *samples, uint8_t count);
```

- `count` must be 1..255. A zero count returns 0.
- The result is rounded to the nearest integer.
- No allocation; cost is O(N).

## Use

Use it for a short ADC burst where one average is enough. Behavior
reference: Padauk `Soft_Filter/AD_filter_03`; this implementation is original.

## Avoid

Do not use it on a continuous stream with impulses; use `trimmed-mean` or
`median`. For a sliding window use `moving-average`.
