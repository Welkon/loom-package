# debounce

Delay a binary state change until the input has stayed stable long enough.

## Contract

```c
void debounce_init(debounce_t *filter, uint16_t threshold_ms);
uint8_t debounce_step(debounce_t *filter, uint8_t raw, uint16_t elapsed_ms);
```

- Call `debounce_step` on a fixed tick; pass the elapsed milliseconds.
- The output changes only after `threshold_ms` of stable input.
- A bounce resets the candidate timer.
- No allocation; state is two bytes plus the threshold.

## Use

Use it for keys, PIR outputs, and presence pins. Behavior reference:
a production target `control.c` (`debounce_state`) and a production target `key.c`; this
implementation is original.

## Avoid

Do not use it on analog values. Use `hysteresis` for thresholds and
`median` or `ema` for noise.
