# moving-average

Average the last N samples with a fixed-size ring buffer.

## Contract

```c
void moving_average_init(moving_average_t *filter, uint16_t *window, uint8_t size);
uint16_t moving_average_step(moving_average_t *filter, uint16_t sample);
void moving_average_reset(moving_average_t *filter);
```

- The caller supplies `window`, an array of `size` elements.
- `size` must be 1..255.
- A `uint32_t` sum holds up to 255 samples of `uint16_t` without overflow.
- The output is rounded to the nearest integer.
- Cost is O(1) per sample.

## Use

Use it for slow signals where a little lag is acceptable, such as battery
voltage or temperature.

## Avoid

Do not use it where an impulse must be removed; one spike still moves the
mean. Use `median` or `trimmed-mean` instead.
