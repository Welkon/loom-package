# weighted-average

Weighted mean of a batch.

## Contract

```c
uint16_t weighted_mean_u16(const uint16_t *samples, const uint8_t *weights, uint8_t count);
```

- `count` must be 1..255.
- Weights are `uint8_t`; a zero weight sum returns 0.
- The result is rounded to the nearest integer.
- No allocation; the product sum fits `uint32_t` for 255 samples.

## Use

Use it when newer or more trusted samples must count more. Behavior
reference: Padauk `Soft_Filter/AD_filter_08`; this implementation is original.

## Avoid

Do not use it for a long stream with drifting weights; keep the weights
fixed and document them.
