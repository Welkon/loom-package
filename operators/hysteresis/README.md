# hysteresis

Keep a binary state while the input stays inside a band.

## Application scenarios

- Battery voltage drives a low-battery flag; the flag must not chatter at the threshold.
- A thermostat turns on below one setpoint and off above another.
- A presence detector must not toggle on noise around its threshold.

## Contract

```c
void hysteresis_init(hysteresis_t *filter, int32_t low, int32_t high, uint8_t initial);
uint8_t hysteresis_step(hysteresis_t *filter, int32_t value);
```

- `low < high`.
- Values at or above `high` set the state.
- Values at or below `low` clear it.
- Inside the band the state is unchanged.
- No allocation.

## Parameters

- `low` / `high`: the band; its width sets noise immunity.
- `initial`: the safe state at boot.

## Memory and timing

- 12 bytes of state; O(1).

## Integration

```c
hysteresis_t battery;
hysteresis_init(&battery, 3200, 3400, 0);
uint8_t low = hysteresis_step(&battery, millivolts());
```

## When not to use

- The state must change only after N samples; add `debounce`.
- The input is analog and must be smoothed; use `ema` first.

## Verification

`make test` covers both thresholds, the band, and the initial state.
Behavior reference: a production target `drivers.c` and a production target `control.c`.
