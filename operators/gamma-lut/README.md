# gamma-lut

Map an input through a small lookup table with linear interpolation.

## Application scenarios

- LED dimming: map a perceptual level to a PWM duty.
- Sensor linearization: correct a nonlinear thermistor or photodiode.
- Any curve stored in flash as a few points, without floating point.

## Contract

```c
uint16_t gamma_lut_apply(const uint16_t *table, uint8_t entries,
                         uint16_t input, uint16_t input_max);
```

- `table` has `entries` points covering `0..input_max`.
- Inputs at or above `input_max` return the last point.
- Between points the result is linearly interpolated and rounded.
- `entries` must be at least 2 for interpolation; 0 or 1 return the first point.
- No allocation; O(1).

## Parameters

- More entries: smoother curve, more flash.
- `input_max` should match the input range; 255, 1023, or 4095 are common.

## Memory and timing

- Table is caller-owned; 2 bytes per entry.
- One multiply, one divide, and two 64-bit products per call.

## Integration

```c
static const uint16_t gamma[5] = {0, 20, 120, 600, 4095};
uint16_t duty = gamma_lut_apply(gamma, 5, level, 255);
```

## When not to use

- A mathematically simple curve that fits in one multiply; skip the table.
- A curve that changes at runtime; rebuild the table or use a different model.

## Verification

`make test` covers endpoints, interpolation, clamping above the range, and tables with fewer than two points.
