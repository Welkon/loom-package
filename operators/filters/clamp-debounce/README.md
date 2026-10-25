# clamp-debounce

Limit each step, then accept a value only after N stable samples.

## Application scenarios

- A slow analog channel is disturbed by spikes and single-sample drift.
- A battery voltage must not change the displayed level on one bad reading.
- A remote measurement arrives with occasional outliers.

## Contract

```c
void clamp_debounce_init(clamp_debounce_t *filter, uint16_t initial, uint16_t max_delta, uint16_t threshold);
uint16_t clamp_debounce_step(clamp_debounce_t *filter, uint16_t sample);
```

- `max_delta` caps the change from the accepted value.
- A clamped value must repeat `threshold` times before it is accepted.
- `threshold = 0` accepts immediately.
- No allocation.

## Parameters

- `max_delta`: largest plausible change per sample.
- `threshold`: number of consistent clamped samples; 3..5 is typical.
- Larger threshold: more rejection, more delay.

## Memory and timing

- 10 bytes of state; O(1) per sample.

## Integration

```c
clamp_debounce_t filter;
clamp_debounce_init(&filter, 1000, 10, 3);
uint16_t value = clamp_debounce_step(&filter, adc_read());
```

## When not to use

- A fast control loop; the clamp and count add delay.
- Only spike rejection is needed; use `median`.

## Verification

`make test` covers the three-sample acceptance, release, zero threshold, and clamping.
Behavior reference: Padauk `Soft_Filter/AD_filter_10`.
