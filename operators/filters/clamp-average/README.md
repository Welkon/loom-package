# clamp-average

Limit each step, then smooth with a moving average.

## Application scenarios

- A noisy actuator feedback can jump, but the physical value moves slowly.
- Battery voltage has load spikes and the gauge needs a smooth value.
- A remote setpoint changes quickly, but the local output must ramp.

## Contract

```c
void clamp_average_init(clamp_average_t *filter, uint16_t initial, uint16_t max_delta, uint16_t *window, uint8_t size);
uint16_t clamp_average_step(clamp_average_t *filter, uint16_t sample);
```

- `max_delta` caps the change from the previous clamped value.
- The clamped value enters a moving-average window.
- The caller owns `window`; `size` must be 1..255.
- No allocation.

## Parameters

- `max_delta`: largest plausible change per sample.
- `size`: smoothing after the clamp; larger is smoother but slower.
- The clamp runs before the average, so a spike is both limited and diluted.

## Memory and timing

- State is 20 bytes plus the caller's `2*size` bytes.
- O(1) per sample.

## Integration

```c
uint16_t window[4];
clamp_average_t filter;
clamp_average_init(&filter, 0, 20, window, 4);
uint16_t value = clamp_average_step(&filter, adc_read());
```

## When not to use

- A real step must pass immediately; the clamp delays it.
- Only one of the two behaviors is needed; use `clamp` or `moving-average`.

## Verification

`make test` covers unclamped averaging, clamping, size 1, and size 0.
Behavior reference: Padauk `Soft_Filter/AD_filter_06`.
