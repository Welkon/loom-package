# adc-sampler

Average N ADC reads on a channel.

## Application scenarios

- Battery voltage and temperature measurements.
- A sensor channel with white noise where one average is enough.
- A low-end MCU that has no hardware oversampling.

## Contract

```c
typedef struct {
    void *context;
    uint16_t (*read)(void *context, uint8_t channel);
} adc_port_t;

void adc_sampler_init(adc_sampler_t *adc, adc_port_t port, uint8_t samples);
uint16_t adc_sampler_read(adc_sampler_t *adc, uint8_t channel);
uint32_t adc_sampler_mv(const adc_sampler_t *adc, uint16_t raw, uint8_t bits, uint32_t vref_mv);
```

- `samples` is 1..255; zero becomes one.
- The average is rounded to the nearest integer.
- `adc_sampler_mv` converts a raw code to millivolts.
- No allocation.

## Parameters

- More samples: less noise, more time. 4..16 is common.
- Pair with `median` or `trimmed-mean` when spikes exist.

## Memory and timing

- 12 bytes of state plus the port context.
- N reads plus one divide; the adapter decides sample time.

## Integration

```c
adc_sampler_t battery;
adc_sampler_init(&battery, board_adc_port(), 8);
uint16_t raw = adc_sampler_read(&battery, 0);
uint32_t mv = adc_sampler_mv(&battery, raw, 12, 3300);
```

## Adapters

- Keep channel setup, reference selection, and sample timing in the adapter.
- Reference examples: a production target `drivers.c`, ZX ADC examples.
- Do not put vendor SFR writes in the core.

## When not to use

- Impulses matter; use `median` or `trimmed-mean` before averaging.
- A hardware oversampling unit already does this; read its result directly.

## Verification

`make test` covers averaging, rounding, zero samples, and millivolt conversion.
