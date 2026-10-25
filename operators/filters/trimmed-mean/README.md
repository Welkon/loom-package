# trimmed-mean

Average a batch after dropping one minimum and one maximum.

## Contract

```c
uint16_t trimmed_mean(const uint16_t *samples, uint8_t count);
```

- `count` must be 3..255. Smaller batches return 0.
- One minimum and one maximum are removed; duplicates are handled as values.
- The result is rounded to the nearest integer.
- No allocation; cost is O(N).

## Use

Use it when a batch can contain one or two impulses, such as PIR or ADC
bursts. The behavior reference is a production target `pir.c` and Padauk
`Soft_Filter/AD_filter_05`; this implementation is original.

## Avoid

Do not use it for a single continuous stream without batching. Use `median`
for a sliding window, or `moving-average` for a cheaper smooth.
