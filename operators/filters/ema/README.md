# ema

First-order lag filter with a power-of-two smoothing factor.

## Contract

```c
void ema_filter_init(ema_filter_t *filter, int32_t initial, uint8_t shift);
int32_t ema_filter_step(ema_filter_t *filter, int32_t sample);
void ema_filter_reset(ema_filter_t *filter, int32_t value);
```

- `shift` is 0..15; the smoothing factor is `2^shift`.
- A larger shift gives more smoothing and more lag.
- `shift = 0` follows the sample exactly.
- Division truncates toward zero, so negative inputs round toward zero.
- Cost is O(1); no allocation.

## Use

Use it for battery voltage, temperature, or a control signal where a
first-order response is enough.

## Avoid

Do not use it when an impulse must be removed; a spike still moves the
output. Use `median` or `trimmed-mean` first.
