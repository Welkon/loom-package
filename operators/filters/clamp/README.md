# clamp

Limit how far one output step may move from the previous output.

## Application scenarios

- An ADC channel jumps because of a bad sample, but the physical signal is continuous.
- A UI or protocol sets a target, and the actuator must move at a bounded rate.
- A sensor is removed or reconnected and the first sample is far from the last good value.

## Contract

```c
void clamp_filter_init(clamp_filter_t *filter, int32_t initial, uint16_t max_delta);
int32_t clamp_filter_step(clamp_filter_t *filter, int32_t sample);
void clamp_filter_reset(clamp_filter_t *filter, int32_t value);
```

- `max_delta` is the largest allowed change per step.
- A larger delta is clamped, not rejected.
- State is one `int32_t` plus the limit; no allocation.

## Parameters

- `initial`: the last known good value.
- `max_delta`: the largest physically plausible change in one sample period.
- Too small: a real step is delayed. Too large: spikes pass.

## Memory and timing

- 8 bytes of state; O(1) per sample.

## Integration

```c
clamp_filter_t filter;
clamp_filter_init(&filter, 0, 10);
int32_t value = clamp_filter_step(&filter, adc_read());
```

## When not to use

- A real step must pass immediately.
- Impulses must be removed: use `median` or `trimmed-mean`.
- The signal needs smoothing: combine with `moving-average` or `ema`.

## Verification

`make test` covers steps, saturation, reset, zero delta, and negative values.
Behavior reference: Padauk `Soft_Filter/AD_filter_01`; this implementation is original.
