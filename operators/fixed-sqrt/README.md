# fixed-sqrt

Integer square root for Q16.16 fixed-point values.

## Application scenarios

- RMS, magnitude, and distance calculations on an MCU without an FPU.
- Sensor linearization that needs a square root.
- Control loops that must avoid floating point.

## Contract

```c
int32_t fixed_sqrt_q16(int32_t value);
```

- Input and output are Q16.16: the integer part is `value >> 16`.
- Negative inputs return 0.
- The result is the truncated integer square root.
- No allocation; no floating point; O(log n).

## Parameters

- Input range is `0..INT32_MAX` (about 32767.999 in Q16.16).
- The output of a valid input fits the Q16.16 range.

## Memory and timing

- No state; one 64-bit loop with about 31 iterations.

## Integration

```c
int32_t rms = fixed_sqrt_q16((a * a + b * b) >> 16);
```

## When not to use

- Values outside the Q16.16 range; use a wider type or scale first.
- A floating-point unit is available and the project allows floats.

## Verification

`make test` covers exact squares, non-squares, zero, and negative input.
Behavior reference: libfixmath `fix16_sqrt` (MIT); this bitwise implementation is original.
