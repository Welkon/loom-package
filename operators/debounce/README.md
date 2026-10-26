# debounce

Delay a binary state change until the input has stayed stable long enough.

## Application scenarios

- A mechanical key or limit switch bounces for a few milliseconds.
- A PIR output or presence pin has short glitches.
- A reed switch chatters at the edge of its actuation range.

## Contract

```c
void debounce_init(debounce_t *filter, uint16_t threshold_ms);
uint8_t debounce_step(debounce_t *filter, uint8_t raw, uint16_t elapsed_ms);
```

- Call on a fixed tick; pass the elapsed milliseconds.
- Output changes only after `threshold_ms` of stable input.
- A bounce resets the candidate timer.
- No allocation.

## Parameters

- `threshold_ms`: 10..30 ms for keys, 50..200 ms for presence.
- `elapsed_ms`: must reflect real time; a fixed 1 ms tick is simplest.

## Memory and timing

- 6 bytes of state; O(1).

## Integration

```c
debounce_t key;
debounce_init(&key, 20);
/* every 1 ms: */
uint8_t pressed = debounce_step(&key, raw_pin(), 1);
```

## When not to use

- Analog values; use `hysteresis` or a filter.
- The caller cannot provide a stable time base.

## Verification

`make test` covers threshold edges, release, bounce reset, and zero threshold.
Behavior reference: production key handlers; this implementation is original.
