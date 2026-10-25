# biquad

Q15 direct-form I biquad section.

## Application scenarios

- Audio tone shaping and anti-alias filters.
- Sensor smoothing where a first-order filter is not enough.
- A fixed-point DSP chain on an MCU without floating point.

## Contract

```c
void biquad_init(biquad_t *filter, int32_t b0, int32_t b1, int32_t b2,
                 int32_t a1, int32_t a2, uint8_t shift);
int32_t biquad_step(biquad_t *filter, int32_t sample);
void biquad_reset(biquad_t *filter);
```

- Coefficients are Q15; `shift` divides the accumulator by `2^shift`.
- Direct form I: `y = (b0*x + b1*x1 + b2*x2 - a1*y1 - a2*y2) / 2^shift`.
- `a1`/`a2` are stored with their normal sign (the equation subtracts them).
- No allocation; state is about 32 bytes.

## Parameters

- Design coefficients with a tool such as SciPy or CMSIS-DSP, then scale to Q15.
- `shift` keeps the accumulator in range; 15 is common when coefficients are Q15.
- Cascade sections for higher-order filters.

## Memory and timing

- 36 bytes; O(1), four 64-bit products per sample.

## Integration

```c
biquad_t lowpass;
biquad_init(&lowpass, 10923, 21845, 10923, -10923, 0, 15);
int32_t filtered = biquad_step(&lowpass, sample);
```

## When not to use

- A single-pole response is enough; use `ema`.
- Coefficient design at runtime; use a floating-point design step offline.

## Verification

`make test` covers unity gain, a three-tap average, impulse response, and reset.
Behavior reference: CMSIS-DSP `arm_biquad_cascade_df1_q15` (Apache-2.0); this implementation is original.
