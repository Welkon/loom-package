# mean

Arithmetic mean of a batch.

## Application scenarios

- A short ADC burst where one average is enough.
- Calibration: average a known reference many times.
- A sensor returns a block of samples and the consumer needs one value.

## Contract

```c
uint16_t mean_u16(const uint16_t *samples, uint8_t count);
```

- `count` must be 1..255. A zero count returns 0.
- Result is rounded to the nearest integer.
- No allocation; O(N).

## Parameters

- Batch size trades noise for latency; 4..16 is common for ADC.
- The sum is `uint32_t`; 255 samples of 65535 fit.

## Memory and timing

- No state; the caller owns the batch.
- One pass; one division at the end.

## Integration

```c
uint16_t samples[8];
/* fill samples, then: */
uint16_t value = mean_u16(samples, 8);
```

## When not to use

- Impulses matter; use `trimmed-mean` or `median`.
- A sliding window is needed; use `moving-average`.

## Verification

`make test` covers rounding, zero count, and maximum values.
Behavior reference: Padauk `Soft_Filter/AD_filter_03`.
