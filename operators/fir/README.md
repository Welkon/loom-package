# fir

Q15 finite-impulse-response filter.

## Application scenarios

- Audio tone shaping and anti-alias filtering.
- Sensor smoothing with a designed response.
- A fixed-point DSP chain on an MCU without floating point.

## Contract

```c
typedef struct {
    const int16_t *taps;
    int16_t *history;
    uint16_t length;
    uint16_t index;
    uint8_t shift;
} fir_t;

void fir_init(fir_t *filter, const int16_t *taps, int16_t *history,
              uint16_t length, uint8_t shift);
int32_t fir_step(fir_t *filter, int16_t sample);
void fir_reset(fir_t *filter);
```

- `taps` and `history` are caller-owned arrays of `length` elements.
- Coefficients are Q15; `shift` divides the accumulator by `2^shift`.
- Direct form: `y[n] = sum(taps[k] * x[n-k]) / 2^shift`.
- No allocation and no floating point.

## Parameters

- Design taps offline with SciPy, MATLAB, or CMSIS-DSP.
- `shift` keeps the accumulator in range; 15 is common.
- A longer filter costs `length` multiply-accumulates per sample.

## Memory and timing

- `history` is `2 * length` bytes; state is about 20 bytes.
- O(length) per sample; a 32-tap filter is 32 MACs.

## Integration

```c
static const int16_t taps[3] = {10923, 10923, 10923};
static int16_t history[3];
fir_t average;
fir_init(&average, taps, history, 3, 15);
int32_t filtered = fir_step(&average, sample);
```

## When not to use

- A low-order response is enough; use `biquad` (constant cost).
- Very long filters or block processing; use an FFT.

## Verification

`make test` covers a two-tap average, a three-tap average, reset, and an
invalid configuration.
Behavior reference: CMSIS-DSP `arm_fir_q15` (Apache-2.0); this implementation is original.
