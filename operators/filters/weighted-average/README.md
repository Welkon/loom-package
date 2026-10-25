# weighted-average

Weighted mean of a batch.

## Application scenarios

- Recent samples are more trustworthy than older ones.
- A sensor fusion step gives different confidence to each source.
- A calibration batch weights the center of the range more.

## Contract

```c
uint16_t weighted_mean_u16(const uint16_t *samples, const uint8_t *weights, uint8_t count);
```

- `count` must be 1..255.
- Weights are `uint8_t`; a zero weight sum returns 0.
- Result is rounded to the nearest integer.
- The product sum fits `uint32_t` for 255 samples with 8-bit weights.
- No allocation; O(N).

## Parameters

- Weights are fixed for the batch; document their meaning.
- All-zero weights return 0; avoid that configuration.

## Memory and timing

- No state; the caller owns both arrays.
- One pass; one division at the end.

## Integration

```c
const uint16_t samples[3] = {10, 20, 30};
const uint8_t weights[3] = {1, 2, 3};
uint16_t value = weighted_mean_u16(samples, weights, 3);
```

## When not to use

- Weights change every sample; use a stateful filter with explicit coefficients.
- Impulses must be removed; use `trimmed-mean` or `median`.

## Verification

`make test` covers a weighted result, zero weights, zero count, and a null input.
Behavior reference: Padauk `Soft_Filter/AD_filter_08`.
