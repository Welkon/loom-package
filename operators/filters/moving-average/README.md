# moving-average

Average the last N samples with a fixed-size ring buffer.

## Application scenarios

- Battery voltage or temperature changes slowly, and the display must not flicker.
- An ADC channel has white noise but no large impulses.
- A control loop needs a simple, bounded-lag smooth.

## Contract

```c
void moving_average_init(moving_average_t *filter, uint16_t *window, uint8_t size);
uint16_t moving_average_step(moving_average_t *filter, uint16_t sample);
void moving_average_reset(moving_average_t *filter);
```

- The caller supplies `window`, an array of `size` elements.
- `size` must be 1..255.
- A `uint32_t` sum holds up to 255 `uint16_t` samples without overflow.
- Output is rounded to the nearest integer.
- O(1) per sample.

## Parameters

- Larger `size`: smoother but slower; the step response reaches about 63% after N samples.
- A spike still moves the mean by `spike/N`; use `median` first if spikes exist.

## Memory and timing

- State is 12 bytes plus the caller's `2*size` bytes.
- O(1) per sample; no division until the output is computed.

## Integration

```c
uint16_t window[8];
moving_average_t filter;
moving_average_init(&filter, window, 8);
uint16_t smooth = moving_average_step(&filter, adc_read());
```

## When not to use

- Impulses matter; one spike contaminates the window.
- Very fast response is required; use `ema` with a small shift.

## Verification

`make test` covers warm-up, wrap, rounding, size 1, size 0, and reset.
Behavior reference: Padauk `Soft_Filter/AD_filter_04`.
