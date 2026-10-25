# ema

First-order lag filter with a power-of-two smoothing factor.

## Application scenarios

- Battery voltage or temperature needs a slow, memory-light smooth.
- A control loop needs a first-order response with a known time constant.
- A display value should follow a trend without jumping.

## Contract

```c
void ema_filter_init(ema_filter_t *filter, int32_t initial, uint8_t shift);
int32_t ema_filter_step(ema_filter_t *filter, int32_t sample);
void ema_filter_reset(ema_filter_t *filter, int32_t value);
```

- `shift` is 0..15; the smoothing factor is `2^shift`.
- `shift = 0` follows the sample exactly.
- Division truncates toward zero, so negative inputs round toward zero.
- O(1); no allocation.

## Parameters

- Time constant is about `2^shift` samples.
- Larger shift: smoother, more lag; smaller shift: faster, more noise.

## Memory and timing

- 8 bytes of state; one shift and one division per sample.

## Integration

```c
ema_filter_t filter;
ema_filter_init(&filter, 0, 3);   // factor 8
int32_t smooth = ema_filter_step(&filter, adc_read());
```

## When not to use

- Impulses must be removed; a spike still moves the output.
- Exact linear-phase filtering is required; use a symmetric FIR.

## Verification

`make test` covers shift 0, positive and negative steps, reset, and shift clamping.
Behavior reference: Padauk `Soft_Filter/AD_filter_07`.
